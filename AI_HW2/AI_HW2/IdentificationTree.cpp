#include "IdentificationTree.h"


/*
	attr1, attr2, attr3, attr4, label �� parse �� ���̺� �߰�
*/
void Parse(string &singleInstance, vvd &attributeTable, vs &labelTable)
{
	int attributeCount = 0;
	vd vectorOfAttribute;
	while (singleInstance.length() != 0 && singleInstance.find(',') != string::npos)
	{
		size_t pos;
		string singleAttribute;
		double data;
		pos = singleInstance.find_first_of(',');
		singleAttribute = singleInstance.substr(0, pos);
		data = stod(singleAttribute);
		vectorOfAttribute.push_back(data);
		singleInstance.erase(0, pos + 1);
	}
	labelTable.push_back(singleInstance);
	attributeTable.push_back(vectorOfAttribute);
	vectorOfAttribute.clear();
}


/* 
	attribute �� �Ǽ����̹Ƿ� �� �߰����� split�� �������� ���Ѵ� .
	return �Ǵ� attributeInfo����
	attributeInfo[0] �� ù��° attribute�� split ������ �ǹ��Ѵ�.
*/
vd GenerateAttributeInfo(vvd &attributeTable)
{
	vd attributeInfo;
	double attr_max_value;
	double attr_min_value;

	for (int i = 0; i < attributeTable[0].size(); ++i)
	{
		attr_max_value = 0;
		attr_min_value = 9999;
		for (int j = 0; j < attributeTable.size(); ++j)
		{
			if (attr_max_value < attributeTable[j][i])
				attr_max_value = attributeTable[j][i];
			if (attr_min_value > attributeTable[j][i])
				attr_min_value = attributeTable[j][i];
		}
		//attr_min_value += 0.5;
		attr_max_value -= 0.7;	// Tuning
		double result = (attr_min_value + attr_max_value) / 2 ;
		attributeInfo.push_back(result);
	}
	return attributeInfo;
}


/*
	Label �� ������ ���� LabelInfo�� ��ȯ�Ѵ�.
	LabelInfo��  Label�� ����
	-- Iris Setosa
	-- Iris Versicolour
	-- Iris Virginica
	�� ���� �����̴�.
*/
vs GenerateLabelInfo(vs &labelTable)
{
	vs labelInfo;
	map<string, int> tempMap;
	for (int i = 0; i < labelTable.size(); ++i)
	{
		tempMap[labelTable[i]]++;
	}

	map<string, int>::iterator iter;
	for (iter = tempMap.begin(); iter != tempMap.end(); ++iter)
	{
		labelInfo.push_back(iter->first);
	}

	return labelInfo;
}


/*
	Table�� ������ �������� Tree �� �����.
*/
node* BuildIdentificationTree(vvd& attributeTable, vs& labelTable, vd& attributeInfo, vs& labelInfo, node* nodePtr, vector<int>& usedAttr)
{
	if (TableIsEmpty(attributeTable, usedAttr))	// �� �̻� ��带 ���� �� ���ٸ� NULL ��ȯ
	{
		return NULL;
	}
		
	if (IsHomogeneous(attributeTable, labelTable))	// homogeneous �ϴٸ� leaf Node�� ��ȯ
	{
		nodePtr->isLeaf = true;
		nodePtr->label = labelTable[0];
		return nodePtr;
	}
	else
	{
		// split ������ �� attribute�� ����
		int splittingAttribute = DecideSplittingAttribute(attributeTable, labelTable, attributeInfo, labelInfo, usedAttr);
		
		// usedAttr �� split �� ����� attribute ���� (����� attribute�� �ٽ� �� �� ����.)
		usedAttr.push_back(splittingAttribute);

		/* 
			���� ����� split ���� attribute�� value ����
			���� ��忡�� ���� �󵵼��� ���� class-label defaultClass ����
		*/
		nodePtr->splitOn = splittingAttribute;
		nodePtr->splitValue = attributeInfo[splittingAttribute];
		nodePtr->defaultClass = ReturnMostFrequentClass(attributeTable, labelTable);
		
		node* child1 = new node;
		node* child2 = new node;

		// child1 �� splitValue ���� ���� ���, else child2
		child1->label = "small";
		child2->label = "large";
		
		child1->isLeaf = false;
		child2->isLeaf = false;

		vvd auxAttrTable1, auxAttrTable2;	// child1�� �� table
		vs auxLabelTable1, auxLabelTable2;	// child2�� �� table
		ChooseTable(attributeTable, auxAttrTable1, labelTable, auxLabelTable1, splittingAttribute, nodePtr->splitValue, child1->label);
		ChooseTable(attributeTable, auxAttrTable2, labelTable, auxLabelTable2, splittingAttribute, nodePtr->splitValue, child2->label);
		// ��ͷ� Ʈ�� Ȯ�� �� children ���Ϳ� �߰�
		nodePtr->children.push_back(BuildIdentificationTree(auxAttrTable1, auxLabelTable1, attributeInfo, labelInfo, child1, usedAttr));
		nodePtr->children.push_back(BuildIdentificationTree(auxAttrTable2, auxLabelTable2, attributeInfo, labelInfo, child2, usedAttr));
	}
	return nodePtr;
}

/*
	Table�� ����ų� ��� attribute�� ����ߴ��� ��ȯ
*/
bool TableIsEmpty(vvd& table, vector<int>& usedAttr)
{
	return (usedAttr.size() == 4 || table.size() == 0);
}

/*
	Entropy�� ����Ͽ� split�� attribute�� ���ϰ� �����Ѵ�.
	information gain = Entropy(root) - (Entropy(attr))
	information gain�� ���� ���� ���� ������ attribute�� �����Ѵ�.
*/
int DecideSplittingAttribute(vvd& attributeTable, vs& labelTable, vd& attributeInfo, vs& labelInfo, vector<int>& usedAttr)
{
	int attr, i, j;
	double minEntropy = DBL_MAX;	// �ּ� Entropy
	int splittingAttr = 0;			// ��ȯȰ attribute

	for (attr = 0; attr < attributeInfo.size(); ++attr)
	{
		// ����� attribute�� skip
		bool skip = false;
		for (j = 0; j < usedAttr.size(); ++j)
		{
			if (attr == usedAttr[j])
				skip = true;
		}
		if (skip) continue;

		/* 
			child1, child2 ���� �б����� ��,
			�� ��忡�� ���� class-label�� size�� ũ�⸦ ����
			���� ���,
			child1_labels_size[0] �� child1 ��忡��
			0 ��° class-label(Iris Setosa) �� ����
		*/
		vector<int> child1_labels_size(labelInfo.size(), 0);
		vector<int> child2_labels_size(labelInfo.size(), 0);

		// child1, child2 ��Ʈ���� 
		double child1_entropy, child2_entropy;
		int root_size, child1_size = 0, child2_size = 0;

		// �ʱ�ȭ
		for (i = 0; i < labelInfo.size(); ++i)
		{
			child1_labels_size[i] = 0;
			child2_labels_size[i] = 0;
		}

		// ���� ����
		for (i = 0; i < labelTable.size(); ++i)
		{
			int index = IndexOfLabel(labelInfo, labelTable[i]);

			if (attributeTable[i][attr] < attributeInfo[attr])
			{
				child1_size += 1;
				child1_labels_size[index] += 1;
			}
			else
			{
				child2_size += 1;
				child2_labels_size[index] += 1;
			}
		}
		root_size = labelTable.size();

		/* 
			child1, child2 ��Ʈ���� ���
			Entrpy = -( p1 * log2(p1) + p2 * log2(p2) + p3 * log2(p3) )
			�Ŀ��� p1, p2, p3�� �� Ŭ������ ��� �󵵼�.

			�ڵ��� p1, p2�� �ٸ� �ǹ̷�, 
			p1�� child1�� i��° Ŭ������ ��� �󵵼��Դϴ�.
		*/
		child1_entropy = 0;
		child2_entropy = 0;
		double noise = 1e-5;
		for (i = 0; i < labelInfo.size(); ++i)
		{
			if (child1_labels_size[i] != 0)
			{
				double p1 = double(child1_labels_size[i]) / child1_size + noise;
				child1_entropy -= p1 * (log(p1) / log(2));
			}
	

			if (child2_labels_size[i] != 0)
			{
				double p2 = double(child2_labels_size[i]) / child2_size + noise;
				child2_entropy -= p2 * (log(p2) / log(2));
			}
		}
	
		/* 
			child ����� ��Ʈ���Ǹ� ����ϰ� 
			���� attribute�� ��Ʈ���� �ѷ� ���
			Entropy(attribute) = c1 * child2_entropy + c2 * child2_entropy)
		*/
		double c1 = child1_size / (double)root_size;
		double c2 = child2_size / (double)root_size;
		double entropy_attr = (c1 * child1_entropy + c2 * child2_entropy);

		// �ּ� ��Ʈ���� ���� �� split attribute ����
		if (minEntropy > entropy_attr)
		{
			minEntropy = entropy_attr;
			splittingAttr = attr;
		}
	}
	
	return splittingAttr;
}

/*  
	labelInfo ���� label �� �ε��� ��ȯ 
*/
int IndexOfLabel(vs& labelInfo, string label)
{
	for (int i = 0; i < labelInfo.size(); ++i)
	{
		if (labelInfo[i] == label)
			return i;
	}

	return -1;
}

/* 
	��� instance�� ������ label�� ������ true �� ����
	�ƴϸ� false ����
*/
bool IsHomogeneous(vvd& attributeTable, vs& labelTable)
{
	int i;
	string label = labelTable[0];
	for (i = 1; i < labelTable.size(); ++i)
	{
		if (label != labelTable[i])
			return false;
	}
	return true;
}

/*
	split ������ �Ǵ� split attribute�� value�� �ش��ϴ� 
	aux_AttributeTable �� aux_LabelTable�� �����.
*/
void ChooseTable(vvd attributeTable, vvd& auxAttrTable, vs labelTable, vs& auxLabelTable, int splittingAttribute, double splitValue, string splitLabel)
{
	int i;
	// split ���� value ���� �۴ٸ�
	if (splitLabel == "small")
	{
		for (i = 0; i < attributeTable.size(); ++i)
		{
			if (attributeTable[i][splittingAttribute] < splitValue)
			{
				auxAttrTable.push_back(attributeTable[i]);
				auxLabelTable.push_back(labelTable[i]);
			}
		}
	}
	// split ���� value ���� ũ�ٸ�
	else
	{
		for (i = 0; i < attributeTable.size(); ++i)
		{
			if (attributeTable[i][splittingAttribute] >= splitValue)
			{
				auxAttrTable.push_back(attributeTable[i]);
				auxLabelTable.push_back(labelTable[i]);
			}
		}
	}
}


/*
	table���� ���� �󵵼��� ���� class-label ��ȯ
*/
string ReturnMostFrequentClass(vvd &attributeTable, vs &labelTable)
{
	// stl map�� ����Ͽ� table�� ��ȸ�ϸ� �ش�Ǵ� class-label�� count + 1
	map<string, int> trainingClasses;
	for (int i = 0; i < labelTable.size(); ++i)
	{
		if (trainingClasses.count(labelTable[i]) == 0)
		{
			trainingClasses[labelTable[i]] = 1;
		}
		else
		{
			trainingClasses[labelTable[i]] += 1;
		}
	}

	// ���� �󵵼��� ���� class-label ã��
	map<string, int>::iterator iter;
	int highestClassCount = 0;
	string mostFrequentClass;
	for (iter = trainingClasses.begin(); iter != trainingClasses.end(); ++iter)
	{
		if (iter->second >= highestClassCount)
		{
			highestClassCount = iter->second;
			mostFrequentClass = iter->first;
		}
	}
	return mostFrequentClass;
}


/*
	�ϳ��� �ν��Ͻ��� Tree �� �׽�Ʈ �غ���
	leaf Node �� �������� �� ����� class-label ����
	���� NULL �� �����Ѵٸ�
	���� �θ� ����� ���� �󵵼��� ���� class-label�� ����.
*/
string TestDataOnTree(vd& singleLine, node* nodePtr)
{
	string prediction;	// ��ȯ�� ���� class-label
	while (!nodePtr->isLeaf && !nodePtr->children.empty())
	{
		int index = nodePtr->splitOn;		// split attribute
		double value = singleLine[index];	// split value
		int childIndex;
		if (value < nodePtr->splitValue)	// ���� �ν��Ͻ��� attribute value�� split value ���� �۴ٸ�
			childIndex = 0;
		else
			childIndex = 1;
		if (nodePtr->children[childIndex] == NULL)	// NULL�� �����Ѵٸ� �θ� ����� default class
		{
			prediction = nodePtr->defaultClass;
			break;
		}
		nodePtr = nodePtr->children[childIndex];	// child ���� ��������
		prediction = nodePtr->label;				// ���� ����� label ����
	}
	return prediction;
}


/*
	Test data Prediction ��� �� ��Ȯ�� ���
*/
void PrintPredictionsAndAccuracy(vs& givenData, vs& predictions)
{
	cout << "*** Test and Predict ***" << endl << endl;
	int correct = 0;
	cout << setw(3) << "#" << setw(16) << "Given Class" << setw(31) << right << "Predicted Class" << endl;
	cout << "--------------------------------------------------" << endl;
	for (int i = 0; i < givenData.size(); ++i)
	{
		cout << setw(3) << i + 1 << setw(16) << givenData[i];
		if (givenData[i] == predictions[i])
		{
			correct += 1;
			cout << "  ------------  ";
		}
		else
		{
			cout << "  XXXXXXXXXXXX  ";
		}
		cout << predictions[i] << endl;
	}
	cout << "--------------------------------------------------" << endl;
	cout << "Total number of instances in test data = " << givenData.size() << endl;
	cout << "Number of Correctly predicted instances = " << correct << endl;
	cout << "Accuracy : " << (double)correct / givenData.size() * 100 << endl;
}


/*
	Ʈ�� ���� ���
	level ��ȸ �� �� �پ� ���
*/
void PrintTree(node* nodePtr)
{
	cout << endl << "** Print Tree **" << endl << endl;

	queue<node*> q;
	vector<node*> v;
	vector<int> lv;
	node* tmp;
	q.push(nodePtr);
	v.push_back(nodePtr);
	
	int level = 0;
	lv.push_back(level);
	level += 1;
	while(!q.empty())
	{
		tmp = q.front();
		q.pop();

		if (!tmp->isLeaf)
		{
			if (tmp->children[0] != NULL)
				q.push(tmp->children[0]);
			if (tmp->children[1] != NULL)
				q.push(tmp->children[1]);
			v.push_back(tmp->children[0]);
			lv.push_back(level);
			v.push_back(tmp->children[1]);
			lv.push_back(level);

			level += 1;
		}
	}

	for (int i = 0; i < v.size(); ++i)
	{
		if (v[i] != NULL)
		{
			if (v[i]->isLeaf)
			{
				cout << "<Leaf Node> class label : " << v[i]->label << ", level : " << lv[i] << endl;
			}
			else
			{
				cout << "<Node> split attr: " <<  v[i]->splitOn << ", level : " << lv[i] << endl;
			}
		}
		else
		{
			cout << "<NULL> X" << ", level : " << lv[i] << endl;
		}
	}
	cout << endl << endl;
}