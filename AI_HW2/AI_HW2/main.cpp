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
	vvd attributeTable;		// attribute data ����
	vs labelTable;			// label data ����

	// ������ ���� ���� ���� ó��
	inputFile.open("train.txt");				
	if (!inputFile)
	{
		cerr << "Error: Training File could not open\n";
		exit(100);
	}

	// �ν��Ͻ� �ϳ��� �о parse
	while (getline(inputFile, singleInstance))
	{
		Parse(singleInstance, attributeTable, labelTable);
	}
	inputFile.close();

	// train data ���
	cout << endl << "*** Print Train Data ***" << endl << endl;
	printData(attributeTable, labelTable);

	// attributeInfo : attr0, attr1, attr2, attr3 ������ �߰��� ������ table
	// labelInfo : �ߺ����� �ʴ� label table 
	vd attributeInfo = GenerateAttributeInfo(attributeTable);
	vs labelInfo = GenerateLabelInfo(labelTable);
	node* root = new node;
	root->isLeaf = false;
	vector<int> usedAttr;

	// Identification Tree �����
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

	vs predictedClassLabels;	// Tree �� ������ �� �ν��Ͻ��� class label ����
	for (int i = 0; i < attributeTable.size(); ++i)
	{
		string predictString = TestDataOnTree(attributeTable[i], root);		// �ϳ��� �ν��Ͻ��� Tree �� �׽�Ʈ �غ��� ���� �������� �� class-label ����
		predictedClassLabels.push_back(predictString);
	}

	PrintTree(root);

	// train data ���
	cout << endl << "*** Print Test Data ***" << endl << endl;
	printData(attributeTable, labelTable);

	PrintPredictionsAndAccuracy(labelTable, predictedClassLabels);


	return 0;
}