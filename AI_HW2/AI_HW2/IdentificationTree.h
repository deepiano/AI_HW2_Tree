#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <math.h>
#include <cstdlib>
#include <iomanip>

using namespace std;

typedef vector<vector<double>> vvd;
typedef vector<double> vd;
typedef vector<string> vs;

/* 
	Data 설명

	attribute = 
		0(sepal_length), 
		1(sepal_width),
		2(petal_length),
		3(petal_width) 

	label(class) =
		-- Iris Setosa
		-- Iris Versicolour
		-- Iris Virginica
*/


// sturct node 는 identification tree의 node를 정의한다.
struct node
{
	int splitOn;					// 어느 attribute 를 기준으로 split 하는지 저장
	string label;					// leaf 노드인 경우 lable 저장, leaf 노드가 아닌 경우 split 기준에서 small / large 인지 저장
	bool isLeaf;					// leaf node 인지 저장
	double splitValue;				// attribute의 기준 value 저장
	vector<node*> children;			// children node 의 포인터 저장
	string defaultClass;			// 현재 노드에서 가장 많은 label
};

void Parse(string&, vvd&, vs&);												// 하나의 인스턴스 parse 해서 table에 저장
vd GenerateAttributeInfo(vvd&);												// attributeInfo (attr0, attr1, attr2, attr3 각각의 splitValue를 저장한 table) 반환
vs GenerateLabelInfo(vs&);													// labelInfo (중복되지 않는 label table) 반환

node* BuildIdentificationTree(vvd&, vs&, vd&, vs&, node*, vector<int>&);	// 트리 만들기
bool TableIsEmpty(vvd&, vector<int>&);										// Table이 비었거나 모든 attribute를 사용했는지 반환

int DecideSplittingAttribute(vvd&, vs&, vd&, vs&, vector<int>&);			// Entropy를 사용하여 split 기준 attribute를 정하여 반환
int IndexOfLabel(vs&, string);												// LabelTabel에서 label string에 해당하는 인덱스 반환
bool IsHomogeneous(vvd&, vs&);												// 현재 table의 class-label이 모두 같은지 판단
void ChooseTable(vvd, vvd&, vs, vs&, int, double, string);					// 현재 table에서 split 기준에 해당하는 instance만 고른 테이블을 새로 만들어 반환

string ReturnMostFrequentClass(vvd&, vs&);									// table에서 가장 빈도수가 높은 class-label을 반환

string TestDataOnTree(vd&, node*);											// 하나의 인스턴스를 Tree 에 테스트 해보고 끝에 도달했을 때 class-label 저장

void PrintPredictionsAndAccuracy(vs&, vs&);									// Test data Prediction 결과 및 정확성 출력
	
void PrintTree(node*);														// 트리 내용 출력