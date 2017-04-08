#pragma once

#include <iostream>
#include <algorithm>
#include <string>
#define UPPER_LIMIT  1.0f

using namespace std;



class Fuzzy {
public:
	Fuzzy();
	~Fuzzy();

	void initializeSystem();
	void getSystemInputs();
	void fuzzification();
	void ruleEvaluation();
	void defuzzification();
	void putSystemOutputs();

	class IOType;

	class MFType {
	public:
		string name;
		float value;
		float point1;
		float point2;
		float slope1;
		float slope2;
		MFType *next;
		IOType *owner;
	};

	class IOType {
	public:
		string name;
		float value;
		MFType *membershipFunctions;
		IOType *next;
	};

	class RuleElementType {
	public:
		MFType *membershipFunction;
		RuleElementType *next;
	};

	class RuleType {
	public:
		RuleElementType *ifSide;
		RuleElementType *thenSide;
		RuleType *next;
	};
	
private:
	MFType* createMF(IOType* owner);
	void computeDegreeOfMembership(MFType *mf, float input);
	float computeAreaOfTrapezoid(MFType *mf);
	void ruleInit();

	IOType *systemInputs;
	IOType *systemOutputs;

	MFType *playerLuckMFs;
	MFType *enemyLuckMFs;
	MFType *counterAttackChanceMFs;

	RuleType *ruleBase;
};

