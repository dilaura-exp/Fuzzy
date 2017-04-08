#include "stdafx.h"
#include "Fuzzy.h"


Fuzzy::Fuzzy() {
}


Fuzzy::~Fuzzy() {
}

void Fuzzy::initializeSystem() {
	cout << "\n----Inisialisasi----\n";
	systemInputs = new IOType[2];
	systemOutputs = new IOType();
	playerLuckMFs = createMF(&systemInputs[0]);
	enemyLuckMFs = createMF(&systemInputs[1]);
	counterAttackChanceMFs = createMF(systemOutputs);
	ruleInit();
}

Fuzzy::MFType* Fuzzy::createMF(IOType* owner) {
	MFType* mfs = new MFType[3];

	mfs[0].name = "Kecil";
	mfs[0].point1 = 0;
	mfs[0].point2 = 50;
	mfs[0].slope1 = 0.04f;
	mfs[0].slope2 = 0.04f;
	mfs[0].next = &mfs[1];
	mfs[0].owner = owner;

	mfs[1].name = "Sedang";
	mfs[1].point1 = 25;
	mfs[1].point2 = 75;
	mfs[1].slope1 = 0.04f;
	mfs[1].slope2 = 0.04f;
	mfs[1].next = &mfs[2];
	mfs[1].owner = owner;

	mfs[2].name = "Besar";
	mfs[2].point1 = 50;
	mfs[2].point2 = 100;
	mfs[2].slope1 = 0.04f;
	mfs[2].slope2 = 0.04f;
	mfs[2].next = nullptr;
	mfs[2].owner = owner;

	return mfs;
}

void Fuzzy::getSystemInputs() {
	systemInputs[0].name = "PlayerLuck";
	systemInputs[0].value = 62;
	systemInputs[0].membershipFunctions = &playerLuckMFs[0];
	systemInputs[0].next = &systemInputs[1];

	cout << "\nInput 1:" << "\nNama: " << systemInputs[0].name << "\nNilai: " << systemInputs[0].value << "\nMembership Functions: ";
	for (MFType *mf = systemInputs[0].membershipFunctions; mf != nullptr; mf = mf->next) {
		cout << mf->name << " ";
	}
	cout << endl;

	systemInputs[1].name = "EnemyLuck";
	systemInputs[1].value = 27;
	systemInputs[1].membershipFunctions = &enemyLuckMFs[0];
	systemInputs[1].next = nullptr;

	cout << "\nInput 2:" << "\nNama: " << systemInputs[1].name << "\nNilai: " << systemInputs[1].value << "\nMembership Functions: ";
	for (MFType *mf = systemInputs[1].membershipFunctions; mf != nullptr; mf = mf->next) {
		cout << mf->name << " ";
	}
	cout << endl;

	systemOutputs->name = "CounterAttackChance";
	systemOutputs->membershipFunctions = &counterAttackChanceMFs[0];
	systemOutputs->next = nullptr;

	cout << "\nOutput:" << "\nNama: " << systemOutputs->name << "\nNilai: ?" << "\nMembership Functions: ";
	for (MFType *mf = systemOutputs->membershipFunctions; mf != nullptr; mf = mf->next) {
		cout << mf->name << " ";
	}
	cout << endl;
}

void Fuzzy::fuzzification() {
	cout << "\n----Fuzifikasi----\n";
	int i = 1;
	for (IOType *systemInput = &systemInputs[0]; systemInput != nullptr; systemInput = systemInput->next) {
		cout << "Input " << i << "(" << systemInput->name << ") :\n";
		for (MFType *membershipFunction = systemInput->membershipFunctions; membershipFunction != nullptr; membershipFunction = membershipFunction->next) {
			computeDegreeOfMembership(membershipFunction, systemInput->value);
			cout << membershipFunction->name << ": " << membershipFunction->value << endl;
		}
		i++; cout << endl;
	}
}

void Fuzzy::computeDegreeOfMembership(MFType *mf, float input) {
	float delta1;
	float delta2;
	delta1 = input - mf->point1;
	delta2 = mf->point2 - input;
	if ((delta1 <= 0) || (delta2 <= 0))
		mf->value = 0;
	else
		mf->value = min((mf->slope1*delta1), (mf->slope2*delta2));
	mf->value = min(mf->value, UPPER_LIMIT);
}

void Fuzzy::ruleEvaluation() {
	cout << "\n----Evaluasi Rules----\n";
	float strength;
	int i = 0;
	for (RuleType *rule = ruleBase; rule != nullptr; rule = rule->next) {
		cout << "\n\nRule-" << i << endl << "IF ";
		strength = UPPER_LIMIT;
		for (RuleElementType *ifParts = &rule->ifSide[0]; ifParts != nullptr; ifParts = ifParts->next) {
			strength = min(strength, ifParts->membershipFunction->value);
			cout << ifParts->membershipFunction->owner->name << " " << ifParts->membershipFunction->name << ": " << ifParts->membershipFunction->value << "  ";
		}
		cout << "strength: " << strength << "\nTHEN ";
		for (RuleElementType *thenParts = rule->thenSide; thenParts != nullptr; thenParts = thenParts->next) {
			thenParts->membershipFunction->value = max(strength, thenParts->membershipFunction->value);
			cout << thenParts->membershipFunction->owner->name << " " << thenParts->membershipFunction->name << ": " << thenParts->membershipFunction->value << "  ";
		}
		i++;
	}
}

float Fuzzy::computeAreaOfTrapezoid(MFType *mf) {
	float run1;
	float run2;
	float base;
	float top;
	float area;
	base = mf->point2 - mf->point1;
	run1 = mf->value / mf->slope1;
	run2 = mf->value / mf->slope2;
	top = base - run1 - run2;
	area = mf->value * (base + top) / 2;
	return area;
}

void Fuzzy::defuzzification() {
	cout << "\n\n----Defuzifikasi----\n";
	float sumOfProducts;
	float sumOfAreas;
	float area;
	float centroid;
	for (IOType *systemOutput = systemOutputs; systemOutput != nullptr; systemOutput = systemOutput->next) {
		sumOfProducts = 0;
		sumOfAreas = 0;
		int i = 1;
		for (MFType *membershipFunction = systemOutput->membershipFunctions; membershipFunction != nullptr; membershipFunction = membershipFunction->next) {
			area = computeAreaOfTrapezoid(membershipFunction);
			centroid = membershipFunction->point1 + (membershipFunction->point2 - membershipFunction->point1) / 2;
			cout << "\nX-axis centroid point " << i << ": " << centroid;
			cout << "\nStrength applied to output " << i << ": " << membershipFunction->value;
			cout << "\nShaded area of output " << i << ": " << area << endl;
			sumOfProducts += area * centroid;
			sumOfAreas += area;
			i++;
		}
		systemOutput->value = sumOfProducts / sumOfAreas;
		cout << "\nWeighted average: " << systemOutput->value << endl;
	}
}

void Fuzzy::putSystemOutputs() {
	cout << "\n----Output----\n";
	cout << "\nName: " << systemOutputs->name << endl << "Value: " << systemOutputs->value << endl;
}

void Fuzzy::ruleInit() {
	ruleBase = new RuleType[9];

	ruleBase[0].ifSide = new RuleElementType[2];
	ruleBase[0].thenSide = new RuleElementType();
	ruleBase[0].ifSide[0].membershipFunction = &playerLuckMFs[0];
	ruleBase[0].ifSide[0].next = &ruleBase[0].ifSide[1];
	ruleBase[0].ifSide[1].membershipFunction = &enemyLuckMFs[0];
	ruleBase[0].ifSide[1].next = nullptr;
	ruleBase[0].thenSide->membershipFunction = &counterAttackChanceMFs[0];
	ruleBase[0].next = &ruleBase[1];

	ruleBase[1].ifSide = new RuleElementType[2];
	ruleBase[1].thenSide = new RuleElementType();
	ruleBase[1].ifSide[0].membershipFunction = &playerLuckMFs[0];
	ruleBase[1].ifSide[0].next = &ruleBase[0].ifSide[1];
	ruleBase[1].ifSide[1].membershipFunction = &enemyLuckMFs[1];
	ruleBase[1].ifSide[1].next = nullptr;
	ruleBase[1].thenSide->membershipFunction = &counterAttackChanceMFs[0];
	ruleBase[1].next = &ruleBase[2];

	ruleBase[2].ifSide = new RuleElementType[2];
	ruleBase[2].thenSide = new RuleElementType();
	ruleBase[2].ifSide[0].membershipFunction = &playerLuckMFs[0];
	ruleBase[2].ifSide[0].next = &ruleBase[0].ifSide[1];
	ruleBase[2].ifSide[1].membershipFunction = &enemyLuckMFs[2];
	ruleBase[2].ifSide[1].next = nullptr;
	ruleBase[2].thenSide->membershipFunction = &counterAttackChanceMFs[0];
	ruleBase[2].next = &ruleBase[3];

	ruleBase[3].ifSide = new RuleElementType[2];
	ruleBase[3].thenSide = new RuleElementType();
	ruleBase[3].ifSide[0].membershipFunction = &playerLuckMFs[1];
	ruleBase[3].ifSide[0].next = &ruleBase[0].ifSide[1];
	ruleBase[3].ifSide[1].membershipFunction = &enemyLuckMFs[0];
	ruleBase[3].ifSide[1].next = nullptr;
	ruleBase[3].thenSide->membershipFunction = &counterAttackChanceMFs[1];
	ruleBase[3].next = &ruleBase[4];

	ruleBase[4].ifSide = new RuleElementType[2];
	ruleBase[4].thenSide = new RuleElementType();
	ruleBase[4].ifSide[0].membershipFunction = &playerLuckMFs[1];
	ruleBase[4].ifSide[0].next = &ruleBase[0].ifSide[1];
	ruleBase[4].ifSide[1].membershipFunction = &enemyLuckMFs[1];
	ruleBase[4].ifSide[1].next = nullptr;
	ruleBase[4].thenSide->membershipFunction = &counterAttackChanceMFs[1];
	ruleBase[4].next = &ruleBase[5];

	ruleBase[5].ifSide = new RuleElementType[2];
	ruleBase[5].thenSide = new RuleElementType();
	ruleBase[5].ifSide[0].membershipFunction = &playerLuckMFs[1];
	ruleBase[5].ifSide[0].next = &ruleBase[0].ifSide[1];
	ruleBase[5].ifSide[1].membershipFunction = &enemyLuckMFs[2];
	ruleBase[5].ifSide[1].next = nullptr;
	ruleBase[5].thenSide->membershipFunction = &counterAttackChanceMFs[0];
	ruleBase[5].next = &ruleBase[6];

	ruleBase[6].ifSide = new RuleElementType[2];
	ruleBase[6].thenSide = new RuleElementType();
	ruleBase[6].ifSide[0].membershipFunction = &playerLuckMFs[2];
	ruleBase[6].ifSide[0].next = &ruleBase[0].ifSide[1];
	ruleBase[6].ifSide[1].membershipFunction = &enemyLuckMFs[0];
	ruleBase[6].ifSide[1].next = nullptr;
	ruleBase[6].thenSide->membershipFunction = &counterAttackChanceMFs[2];
	ruleBase[6].next = &ruleBase[7];

	ruleBase[7].ifSide = new RuleElementType[2];
	ruleBase[7].thenSide = new RuleElementType();
	ruleBase[7].ifSide[0].membershipFunction = &playerLuckMFs[2];
	ruleBase[7].ifSide[0].next = &ruleBase[0].ifSide[1];
	ruleBase[7].ifSide[1].membershipFunction = &enemyLuckMFs[1];
	ruleBase[7].ifSide[1].next = nullptr;
	ruleBase[7].thenSide->membershipFunction = &counterAttackChanceMFs[1];
	ruleBase[7].next = &ruleBase[8];

	ruleBase[8].ifSide = new RuleElementType[2];
	ruleBase[8].thenSide = new RuleElementType();
	ruleBase[8].ifSide[0].membershipFunction = &playerLuckMFs[2];
	ruleBase[8].ifSide[0].next = &ruleBase[0].ifSide[1];
	ruleBase[8].ifSide[1].membershipFunction = &enemyLuckMFs[2];
	ruleBase[8].ifSide[1].next = nullptr;
	ruleBase[8].thenSide->membershipFunction = &counterAttackChanceMFs[1];
	ruleBase[8].next = nullptr;
}