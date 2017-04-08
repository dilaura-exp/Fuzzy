
#include "stdafx.h"
#include "Fuzzy.h"

int main() {
	Fuzzy fuzzy;
	fuzzy.initializeSystem();
	fuzzy.getSystemInputs();
	fuzzy.fuzzification();
	fuzzy.ruleEvaluation();
	fuzzy.defuzzification();
	fuzzy.putSystemOutputs();

	system("PAUSE");
	return 0;
}