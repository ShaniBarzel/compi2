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

/*** CheckNullRule by Shani and Racheli
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


/**
 * input: -
 * output: a vector that shows for each non terminal if it is nullable
 */
std::vector<bool> IsNullableNon(){
    std::vector<bool> IsNonterminalsNullable(); //answer vector
    //init answer vector to false
    for (int i=0; i< NONTERMINAL_ENUM_SIZE; i++){
        IsNonterminalsNullable().insert(false);
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

        if (IsNonterminalsNullable()[nonterminal] == true){
            //no need to check the non-terminal, we already know it is nullable
            nonterminal++;
            continue;
        }

        // iterate over all the rules
        for (std::vector<grammar_rule>::const_iterator it = grammar.begin(); it != grammar.end(); it++) {
            if ( ((*it).lhs == nonterminal) && (CheckNullRule(*it,IsNonterminalsNullable()))) {
                // if the currnet rule starts with the currnet nonterminal and the rule is nullable, then the current nonterminal is nullable
                IsNonterminalsNullable()[nonterminal] = true;
                nothing_changed = false; //if we reached here, than the current non-terminal was not nullable
                break; // (from for loop)
            }
        }

        nonterminal++;
    }
    return IsNonterminalsNullable();
}


/**
 * input: -
 * output: a vector, containing the first of all variabls
 */
std::vector<std::set<tokens>> First(){
    std::vector<std::set<tokens >>& vec();
    vec().reserve(EF); //init vector to the size of terminals + non_terminals
    //init all the first's of the tokens to contain themselves
    for (int token = KEY; token <EF; token++){
        vec()[token].insert(token);
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

        // iterates over all the rules
        for (std::vector<grammar_rule>::const_iterator it = grammar.begin(); it != grammar.end(); it++) {
            if  ((*it).lhs == nonterminal) {
                // if the currnet rule left section is the currnet nonterminal
                for(std::vector<int>::const_iterator R_it = (*it).rhs.begin(); R_it!=(*it).rhs.end(); R_it++) {
                    int current_first_size = vec()[nonterminal].size();
                    //unite the "firsts" the nonterminals
                    std::set_union(vec()[nonterminal].begin(), vec()[nonterminal].end() ,vec()[*R_it].begin(),
                                   vec()[*R_it].end(),std::inserter(vec()[nonterminal],vec()[nonterminal].begin()));

                    int new_first_size = vec()[nonterminal].size();

                    if (new_first_size != current_first_size)
                        nothing_changed = false; //if we reached here, than the current non-terminal first's was updated

                    // if the current variable is not nullable, the rest of the right secion variables is irrelevent
                    if (!IsNullableNon(*R_it))
                        break; // (from inner for)
                }
            }
        }

        nonterminal++;
    }
    return vec();
}


/**
 * input: a clause as int vector
 * output: the clause "first"
 */
std::set<tokens> ClauseFirst(std::vector<int> clause){
    std::set<tokens> first(); // return vactor
    std::vector<std::set<tokens>> VarFirst = First();
    for(int i = 0; i<clause.size(); i++) {
        //for all terminals and non terminals in the clause, unite them with the current First of the clause
        std::set_union(first().begin(), first().end() ,VarFirst[i].begin(),
                       VarFirst()[i].end(),std::inserter(first(),first().begin()));
    }
    return first();
}


/**
 * input: a clause as int vector
 * output: true if the clause is nullable
 */
bool IsNullableClause(std::vector<int> clause){
    std::vector<bool> IsNull = IsNullableNon();
    for(int i = 0; i<clause.size(); i++) {
        //if one of the variables in the clause is nullable, the clause is not nullable
        if (i>NONTERMINAL_ENUM_SIZE || !IsNull[i])
            return false;
    }
    return true;
}


/** ======================================== GIVEN FUNCTION STARTS HERE =============================================**/

/**
 * determines which variables are nullable, i.e. can produce an empty word
 * calls print_nullable when finished
 */
void compute_nullable(){
    std::vector<bool> IsNonterminalsNullable = IsNullableNon(); //answer vector
    //init answer vector to false
    print_nullable(IsNonterminalsNullable());
}

/**
 * computes first for all nonterminal (see nonterminal enum in grammar.h)
 * calls print_first when finished
 */
void compute_first(){
    std::vector<std::set<tokens >>& vec() = First();
    print_first(vec());
}


/**
 * computes follow for all nonterminal (see nonterminal enum in grammar.h)
 * calls print_follow when finished
 */
void compute_follow();

/**
 * computes select for all grammar rules (see grammar global variable in grammar.h)
 * calls print_select when finished
 */
void compute_select(){
    std::vector<std::set<tokens> >& vec();
    vec().reserve(grammar.size());

    for (std::vector<grammar_rule>::const_iterator it = grammar.begin(); it != grammar.end(); it++) {
            if (IsNullableClause(((*it).rhs))){
                vec().insert(ClauseFirst((*it).rhs))
            }
            else{
                vec().insert(ClauseFirst((*it).rhs));
            }
    }
}
/**
 * implements an LL(1) parser for the grammar using yylex()
 */
void parser();
/*
 * implemented in lex.yy.c (generated from lexer.lex)
 */
int yylex();

#endif //__HW2__