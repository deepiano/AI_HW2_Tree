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
	Data ����

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


// sturct node �� identification tree�� node�� �����Ѵ�.
struct node
{
	int splitOn;					// ��� attribute �� �������� split �ϴ��� ����
	string label;					// leaf ����� ��� lable ����, leaf ��尡 �ƴ� ��� split ���ؿ��� small / large ���� ����
	bool isLeaf;					// leaf node ���� ����
	double splitValue;				// attribute�� ���� value ����
	vector<node*> children;			// children node �� ������ ����
	string defaultClass;			// ���� ��忡�� ���� ���� label
};

void Parse(string&, vvd&, vs&);												// �ϳ��� �ν��Ͻ� parse �ؼ� table�� ����
vd GenerateAttributeInfo(vvd&);												// attributeInfo (attr0, attr1, attr2, attr3 ������ splitValue�� ������ table) ��ȯ
vs GenerateLabelInfo(vs&);													// labelInfo (�ߺ����� �ʴ� label table) ��ȯ

node* BuildIdentificationTree(vvd&, vs&, vd&, vs&, node*, vector<int>&);	// Ʈ�� �����
bool TableIsEmpty(vvd&, vector<int>&);										// Table�� ����ų� ��� attribute�� ����ߴ��� ��ȯ

int DecideSplittingAttribute(vvd&, vs&, vd&, vs&, vector<int>&);			// Entropy�� ����Ͽ� split ���� attribute�� ���Ͽ� ��ȯ
int IndexOfLabel(vs&, string);												// LabelTabel���� label string�� �ش��ϴ� �ε��� ��ȯ
bool IsHomogeneous(vvd&, vs&);												// ���� table�� class-label�� ��� ������ �Ǵ�
void ChooseTable(vvd, vvd&, vs, vs&, int, double, string);					// ���� table���� split ���ؿ� �ش��ϴ� instance�� �� ���̺��� ���� ����� ��ȯ

string ReturnMostFrequentClass(vvd&, vs&);									// table���� ���� �󵵼��� ���� class-label�� ��ȯ

string TestDataOnTree(vd&, node*);											// �ϳ��� �ν��Ͻ��� Tree �� �׽�Ʈ �غ��� ���� �������� �� class-label ����

void PrintPredictionsAndAccuracy(vs&, vs&);									// Test data Prediction ��� �� ��Ȯ�� ���
	
void PrintTree(node*);														// Ʈ�� ���� ���