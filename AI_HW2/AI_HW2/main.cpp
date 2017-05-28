#include <iostream>
#include <string>

#include "IdentificationTree.h"
using namespace std;

void printData(vvd& attr, vs label)
{
	cout << setw(3) << "#"
		<< setw(15) << "sepal_length"
		<< setw(15) << "sepal_width"
		<< setw(15) << "petal length"
		<< setw(15) << "peatl width"
		<< setw(20) << "Class Label" << endl;
	cout << "------------------------------------------";
	cout << "------------------------------------------" << endl;
	for (int i = 0; i < attr.size(); ++i)
	{
		cout << setw(3) << i+1
			<< setw(15) << attr[i][0]
			<< setw(15) << attr[i][1]
			<< setw(15) << attr[i][2]
			<< setw(15) << attr[i][3]
			<< setw(20) << label[i] << endl;
	}
	cout << endl << endl;
}

int main()
{
	ifstream inputFile;
	string singleInstance;
	vvd attributeTable;		// attribute data 저장
	vs labelTable;			// label data 저장

	// 데이터 파일 접근 에러 처리
	inputFile.open("train.txt");				
	if (!inputFile)
	{
		cerr << "Error: Training File could not open\n";
		exit(100);
	}

	// 인스턴스 하나씩 읽어서 parse
	while (getline(inputFile, singleInstance))
	{
		Parse(singleInstance, attributeTable, labelTable);
	}
	inputFile.close();

	// train data 출력
	cout << endl << "*** Print Train Data ***" << endl << endl;
	printData(attributeTable, labelTable);

	// attributeInfo : attr0, attr1, attr2, attr3 각각의 중간값 저장한 table
	// labelInfo : 중복되지 않는 label table 
	vd attributeInfo = GenerateAttributeInfo(attributeTable);
	vs labelInfo = GenerateLabelInfo(labelTable);
	node* root = new node;
	root->isLeaf = false;
	vector<int> usedAttr;

	// Identification Tree 만들기
	root = BuildIdentificationTree(attributeTable, labelTable, attributeInfo, labelInfo, root, usedAttr);
	attributeTable.clear();
	labelTable.clear();

	inputFile.clear();
	inputFile.open("test.txt");
	if (!inputFile)
	{
		cerr << "Error : test file could not open\n";
		exit(100);
	}
	
	while (getline(inputFile, singleInstance))
	{
		Parse(singleInstance, attributeTable, labelTable);
	}

	vs predictedClassLabels;	// Tree 가 예측한 각 인스턴스의 class label 저장
	for (int i = 0; i < attributeTable.size(); ++i)
	{
		string predictString = TestDataOnTree(attributeTable[i], root);		// 하나의 인스턴스를 Tree 에 테스트 해보고 끝에 도달했을 때 class-label 저장
		predictedClassLabels.push_back(predictString);
	}

	PrintTree(root);

	// train data 출력
	cout << endl << "*** Print Test Data ***" << endl << endl;
	printData(attributeTable, labelTable);

	PrintPredictionsAndAccuracy(labelTable, predictedClassLabels);


	return 0;
}