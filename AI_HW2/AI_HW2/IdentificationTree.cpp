#include "IdentificationTree.h"


/*
	attr1, attr2, attr3, attr4, label 로 parse 각 테이블에 추가
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
	attribute 가 실수값이므로 그 중간값을 split의 기준으로 정한다 .
	return 되는 attributeInfo에서
	attributeInfo[0] 이 첫번째 attribute의 split 기준을 의미한다.
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
	Label 의 정보를 담은 LabelInfo를 반환한다.
	LabelInfo는  Label의 종류
	-- Iris Setosa
	-- Iris Versicolour
	-- Iris Virginica
	를 담은 벡터이다.
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
	Table의 정보를 바탕으로 Tree 를 만든다.
*/
node* BuildIdentificationTree(vvd& attributeTable, vs& labelTable, vd& attributeInfo, vs& labelInfo, node* nodePtr, vector<int>& usedAttr)
{
	if (TableIsEmpty(attributeTable, usedAttr))	// 더 이상 노드를 만들 수 없다면 NULL 반환
	{
		return NULL;
	}
		
	if (IsHomogeneous(attributeTable, labelTable))	// homogeneous 하다면 leaf Node로 반환
	{
		nodePtr->isLeaf = true;
		nodePtr->label = labelTable[0];
		return nodePtr;
	}
	else
	{
		// split 기준이 될 attribute를 저장
		int splittingAttribute = DecideSplittingAttribute(attributeTable, labelTable, attributeInfo, labelInfo, usedAttr);
		
		// usedAttr 는 split 에 사용한 attribute 저장 (사용한 attribute는 다시 쓸 수 없다.)
		usedAttr.push_back(splittingAttribute);

		/* 
			현재 노드의 split 기준 attribute와 value 저장
			현재 노드에서 가장 빈도수가 높은 class-label defaultClass 저장
		*/
		nodePtr->splitOn = splittingAttribute;
		nodePtr->splitValue = attributeInfo[splittingAttribute];
		nodePtr->defaultClass = ReturnMostFrequentClass(attributeTable, labelTable);
		
		node* child1 = new node;
		node* child2 = new node;

		// child1 은 splitValue 보다 작은 노드, else child2
		child1->label = "small";
		child2->label = "large";
		
		child1->isLeaf = false;
		child2->isLeaf = false;

		vvd auxAttrTable1, auxAttrTable2;	// child1의 새 table
		vs auxLabelTable1, auxLabelTable2;	// child2의 새 table
		ChooseTable(attributeTable, auxAttrTable1, labelTable, auxLabelTable1, splittingAttribute, nodePtr->splitValue, child1->label);
		ChooseTable(attributeTable, auxAttrTable2, labelTable, auxLabelTable2, splittingAttribute, nodePtr->splitValue, child2->label);
		// 재귀로 트리 확장 및 children 벡터에 추가
		nodePtr->children.push_back(BuildIdentificationTree(auxAttrTable1, auxLabelTable1, attributeInfo, labelInfo, child1, usedAttr));
		nodePtr->children.push_back(BuildIdentificationTree(auxAttrTable2, auxLabelTable2, attributeInfo, labelInfo, child2, usedAttr));
	}
	return nodePtr;
}

/*
	Table이 비었거나 모든 attribute를 사용했는지 반환
*/
bool TableIsEmpty(vvd& table, vector<int>& usedAttr)
{
	return (usedAttr.size() == 4 || table.size() == 0);
}

/*
	Entropy를 사용하여 split할 attribute를 정하고 리턴한다.
	information gain = Entropy(root) - (Entropy(attr))
	information gain이 가장 높은 값이 나오는 attribute를 선택한다.
*/
int DecideSplittingAttribute(vvd& attributeTable, vs& labelTable, vd& attributeInfo, vs& labelInfo, vector<int>& usedAttr)
{
	int attr, i, j;
	double minEntropy = DBL_MAX;	// 최소 Entropy
	int splittingAttr = 0;			// 반환활 attribute

	for (attr = 0; attr < attributeInfo.size(); ++attr)
	{
		// 사용한 attribute는 skip
		bool skip = false;
		for (j = 0; j < usedAttr.size(); ++j)
		{
			if (attr == usedAttr[j])
				skip = true;
		}
		if (skip) continue;

		/* 
			child1, child2 노드로 분기했을 때,
			각 노드에서 개별 class-label의 size의 크기를 저장
			예를 들면,
			child1_labels_size[0] 은 child1 노드에서
			0 번째 class-label(Iris Setosa) 의 갯수
		*/
		vector<int> child1_labels_size(labelInfo.size(), 0);
		vector<int> child2_labels_size(labelInfo.size(), 0);

		// child1, child2 엔트로피 
		double child1_entropy, child2_entropy;
		int root_size, child1_size = 0, child2_size = 0;

		// 초기화
		for (i = 0; i < labelInfo.size(); ++i)
		{
			child1_labels_size[i] = 0;
			child2_labels_size[i] = 0;
		}

		// 갯수 저장
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
			child1, child2 엔트로피 계산
			Entrpy = -( p1 * log2(p1) + p2 * log2(p2) + p3 * log2(p3) )
			식에서 p1, p2, p3는 각 클래스의 상대 빈도수.

			코드의 p1, p2는 다른 의미로, 
			p1은 child1의 i번째 클래스의 상대 빈도수입니다.
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
			child 노드의 엔트로피를 계산하고 
			현재 attribute의 엔트로피 총량 계산
			Entropy(attribute) = c1 * child2_entropy + c2 * child2_entropy)
		*/
		double c1 = child1_size / (double)root_size;
		double c2 = child2_size / (double)root_size;
		double entropy_attr = (c1 * child1_entropy + c2 * child2_entropy);

		// 최소 엔트로피 저장 및 split attribute 갱신
		if (minEntropy > entropy_attr)
		{
			minEntropy = entropy_attr;
			splittingAttr = attr;
		}
	}
	
	return splittingAttr;
}

/*  
	labelInfo 에서 label 의 인덱스 반환 
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
	모든 instance가 동일한 label을 가지면 true 를 리턴
	아니면 false 리턴
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
	split 기준이 되는 split attribute의 value에 해당하는 
	aux_AttributeTable 과 aux_LabelTable을 만든다.
*/
void ChooseTable(vvd attributeTable, vvd& auxAttrTable, vs labelTable, vs& auxLabelTable, int splittingAttribute, double splitValue, string splitLabel)
{
	int i;
	// split 기준 value 보다 작다면
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
	// split 기준 value 보다 크다면
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
	table에서 가장 빈도수가 높은 class-label 반환
*/
string ReturnMostFrequentClass(vvd &attributeTable, vs &labelTable)
{
	// stl map을 사용하여 table을 순회하며 해당되는 class-label의 count + 1
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

	// 가장 빈도수가 높은 class-label 찾기
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
	하나의 인스턴스를 Tree 에 테스트 해보고
	leaf Node 에 도달했을 때 노드의 class-label 저장
	만약 NULL 에 도달한다면
	이전 부모 노드의 가장 빈도수가 높은 class-label을 선택.
*/
string TestDataOnTree(vd& singleLine, node* nodePtr)
{
	string prediction;	// 반환할 예측 class-label
	while (!nodePtr->isLeaf && !nodePtr->children.empty())
	{
		int index = nodePtr->splitOn;		// split attribute
		double value = singleLine[index];	// split value
		int childIndex;
		if (value < nodePtr->splitValue)	// 현재 인스턴스의 attribute value가 split value 보다 작다면
			childIndex = 0;
		else
			childIndex = 1;
		if (nodePtr->children[childIndex] == NULL)	// NULL에 도달한다면 부모 노드의 default class
		{
			prediction = nodePtr->defaultClass;
			break;
		}
		nodePtr = nodePtr->children[childIndex];	// child 노드로 내려가기
		prediction = nodePtr->label;				// 현재 노드의 label 선택
	}
	return prediction;
}


/*
	Test data Prediction 결과 및 정확성 출력
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
	트리 내용 출력
	level 순회 후 한 줄씩 출력
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