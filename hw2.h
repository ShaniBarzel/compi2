#ifndef __HW2__
#define __HW2__

#include "tokens.h"
#include "grammar.h"

/**
*** ContainsTerminals by Shani and Racheli
 * input: terminals and nonterminals vector
 * output: if the vector contains only terminals
 */
bool ContainsTerminals(std::vector<int> tavnit){
    std::vector<int>::const_iterator it = tavnit.begin();
    if (*it >= 11){ // nonterminals enumeration is only 0 to 10
        return true;
    }
    it++;
}


/**
*** CheckNullRule by Shani and Racheli
 * input: Grammer Rule
 * output: true if the rule is "nullable" = the variable in the right section of the rule, can derive epsilon by using this rule
 */
 bool CheckNullRule(grammar_rule Gr){
    if (Gr.rhs().empty())
        return true; //the rule has only epsilon in the right section

    if (ContainsTerminals(Gr.rhs()))
        return false; //the rule contains terminal in the right section (so the rule is not nullable)

    //if we reached here, the rule contains only non-terminals in the right section
    int  is_null = 0; //counter of the nullable non-terminals in the right section of the rule

    //iterate all the non-terminals in the right section of the rule
    for ( std::vector<int>::const_iterator R_it = Gr.rhs().begin(); R_it != Gr.rhs.end(); R_it++){
        // iterate all the rules
        for(std::vector<grammar_rule>::const_iterator Rules_it = grammar.begin(); Rules_it != grammar.end(); Rules_it++){
            // if the rule left section is the current non-terminal from the right section of Gr, check if it is nullable
            if ( ((*Rules_it).lhs == *R_it) && (Gr != *Rules_it) ) { //second condition is for avoiding endless recursion
                if (CheckNullRule(*Rules_it)) {
                    is_null++; //the current non-terminal of the right section of Gr is nullable
                    break; //(from for loop)
                } //if the current rule is not nullable, but other rule is nullable, the non-terminal is nullable
            }
        }
    }
    if (is_null == Gr.rhs().size()) //all the non-terminals in the right section of Gr are nullable
        return true;
}


/** =================== Fixed Point Version
*** CheckNullRule by Shani and Racheli
 * input: Grammer Rule and a vector that contains for each non terminal if it is nullable
 * output: true if the rule is "nullable" = the variable in the right section of the rule, can derive epsilon by using this rule
 */
bool CheckNullRule(grammar_rule Gr, std::vector<bool> IsNonterminalsNullable){
    if (Gr.rhs().empty())
        return true; //the rule has only epsilon in the right section

    if (ContainsTerminals(Gr.rhs()))
        return false; //the rule contains terminal in the right section (so the rule is not nullable)

    //if we reached here, the rule contains only non-terminals in the right section

    //iterate all the non-terminals in the right section of the rule
    for ( std::vector<int>::const_iterator R_it = Gr.rhs().begin(); R_it != Gr.rhs.end(); R_it++){
       if (IsNonterminalsNullable[*R_it] == false)
           return false; //one of the non-terminals in the right section is not nullable
    }
    return true;
}

/** ======================================== GIVEN FUNCTION STARTS HERE =============================================**/

/**
 * determines which variables are nullable, i.e. can produce an empty word
 * calls print_nullable when finished
 */
void compute_nullable(){
    std::vector<bool> IsNonterminalsNullable(); //answer vector
    int nonterminal = S;
    // iterate all the nonterminals
    while(nonterminal != NONTERMINAL_ENUM_SIZE){
        // iterate all the rules
        for (std::vector<grammar_rule>::const_iterator it = grammar.begin(); it != grammar.end(); it++) {
            if ( ((*it).lhs == nonterminal) && (CheckNullRule(*it))) {
                // if the currnet rule starts with the currnet nonterminal and the rule is nullable, then the current nonterminal is nullable
                IsNonterminalsNullable().insert(true);
                break; // (from for loop)
            }
        }
        if (IsNonterminalsNullable().size() < nonterminal){
            // if no bool value added to the answer vector for the currnt nonterminal, then non of the rules that starts with it are nullable
            IsNonterminalsNullable().insert(false);
        }
        nonterminal++;
    }
    print_nullable(IsNonterminalsNullable);
}

/** =================== Fixed Point Version
/**
 * determines which variables are nullable, i.e. can produce an empty word
 * calls print_nullable when finished
 */
void compute_nullable(){
    std::vector<bool> IsNonterminalsNullable(); //answer vector
    //init answer vector to false
    for (int i=0; i< NONTERMINAL_ENUM_SIZE; i++){
        IsNonterminalsNullable.insert(false);
    }

    bool nothing_changed = true;
    bool done = false;
    int nonterminal = S;
    // iterate all the nonterminals, while there is still changes
    while(!done){
        //if we are done iterating over all the non-terminlas
        if (nonterminal == NONTERMINAL_ENUM_SIZE){
            if (nothing_changed == true){
                //no changes has done compering to last iteration over all the non-terminal - no more updates
                done = true;
                continue;
            }
            else{
                //there was some changes compering to the last iteration, start itarating over all the non terminal once again
                nothing_changed = true;
                nonterminal = S;
            }
        }

        if (IsNonterminalsNullable[nonterminal] == true){
            //no need to check the non-terminal, we already know it is nullable
            nonterminal++;
            continue;
        }

        // iterate over all the rules
        for (std::vector<grammar_rule>::const_iterator it = grammar.begin(); it != grammar.end(); it++) {
            if ( ((*it).lhs == nonterminal) && (CheckNullRule(*it,IsNonterminalsNullable))) {
                // if the currnet rule starts with the currnet nonterminal and the rule is nullable, then the current nonterminal is nullable
                IsNonterminalsNullable[nonterminal] = true;
                nothing_changed = false; //if we reached here, than the current non-terminal was not nullable
                break; // (from for loop)
            }
        }

        nonterminal++;
    }
    print_nullable(IsNonterminalsNullable);
}

/**
 * computes first for all nonterminal (see nonterminal enum in grammar.h)
 * calls print_first when finished
 */
void compute_first();

/**
 * computes follow for all nonterminal (see nonterminal enum in grammar.h)
 * calls print_follow when finished
 */
void compute_follow();

/**
 * computes select for all grammar rules (see grammar global variable in grammar.h)
 * calls print_select when finished
 */
void compute_select();

/**
 * implements an LL(1) parser for the grammar using yylex()
 */
void parser();
/*
 * implemented in lex.yy.c (generated from lexer.lex)
 */
int yylex();

#endif //__HW2__