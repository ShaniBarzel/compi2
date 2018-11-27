#include <algorithm>
#include <map>
#include <iostream>
#include "hw2.h"
using namespace std;

/**
*** ContainsTerminals by Shani and Racheli
 * input: terminals and nonterminals vector
 * output: if the vector contains a terminal
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
    if (Gr.rhs.empty())
        return true; //the rule has only epsilon in the right section

    //contain Terminals did not work here so i has to do this:
    for(int var=0; var<Gr.rhs.size(); var++){
        if(Gr.rhs[var]>=11)
            return false; //the rule contains terminal in the right section (so the rule is not nullable)
    }

    //if we reached here, the rule contains only non-terminals in the right section

    //iterate all the non-terminals in the right section of the rule
    for ( std::vector<int>::const_iterator R_it = Gr.rhs.begin(); R_it != Gr.rhs.end(); R_it++){
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
    std::vector<bool> IsNonterminalsNullable; //answer vector
   // IsNonterminalsNullable.reserve(NONTERMINAL_ENUM_SIZE);
    //init answer vector to false
    for (int i=0; i< NONTERMINAL_ENUM_SIZE; i++){
        IsNonterminalsNullable.push_back(false);
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
   // print_nullable(IsNonterminalsNullable); //for debug
    return IsNonterminalsNullable;
}


/**
 * input: -
 * output: a vector, containing the first of all variabls
 */
std::vector<std::set<tokens> > First(){
    std::vector<bool> IsNull = IsNullableNon();
    std::vector<std::set<tokens> > vec;     //debug rach
  // vec.reserve(EF); //init vector to the size of terminals + non_terminals
   for(int i=0;i<EF;i++){
        vec.insert(vec.begin()+i,std::set<tokens>());
    }
    //init all the first's of the tokens to contain themselves
    for (int token = KEY; token <EF; token++){
        vec[token].insert(tokens(token));
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
                //std::set<tokens>::iterator temp = vec.operator[](nonterminal).begin();

                for(std::vector<int>::const_iterator R_it = (*it).rhs.begin(); R_it!=(*it).rhs.end(); R_it++) {
                    int current_first_size = vec[nonterminal].size();
                    //unite the "firsts" of the nonterminals
                    std::set_union(vec[nonterminal].begin(), vec[nonterminal].end() ,vec[*R_it].begin(),
                                   vec[*R_it].end(),std::inserter(vec[nonterminal],vec[nonterminal].begin()));

                    int new_first_size = vec[nonterminal].size();

                    if (new_first_size != current_first_size)
                        nothing_changed = false; //if we reached here, than the current non-terminal first's was updated

                    // if the current variable is not nullable, the rest of the right secion variables is irrelevent
                    if ((*R_it)>10 || !IsNull[*R_it]) //SHANI
                        break; // (from inner for)
                }
            }
        }

        nonterminal++;
    }
 //   print_first(vec); //for debug
    return vec;
}


/**
 * input: a clause as int vector
 * output: the clause "first"
 */
std::set<tokens> ClauseFirst(std::vector<int> clause){
    std::vector<bool> IsNull = IsNullableNon();
    std::set<tokens > first; // return vactor
    std::vector<std::set<tokens> > VarFirst = First();
    for(int i = 0; i<clause.size(); i++) {
        //if we reached here, current var in the right section is nullable
        std::set_union(first.begin(), first.end() ,VarFirst[clause[i]].begin(),
                       VarFirst[clause[i]].end(),std::inserter(reinterpret_cast<std::set<tokens> &>(first),first.begin()));

        // if the current variable is not nullable, the rest of the right secion variables is irrelevent
        if (clause[i]>10 || !IsNull[clause[i]]) //SHANI
            break; // (from inner for)
    }
    return first;
}


/**
 * input: a clause as int vector
 * output: true if the clause is nullable
 */
bool IsNullableClause(std::vector<int> Clause){
    std::vector<bool> IsNull = IsNullableNon();
    for(int i = 0; i<Clause.size(); i++) {
        //if one of the variables in the clause is nullable, the clause is not nullable
        if (i>NONTERMINAL_ENUM_SIZE || !IsNull[i])
            return false;
    }
    return true;
}

/**
 * input: -
 * output: a vector that contains the follow of each Non Terminal
 */
std::vector<std::set<tokens> > Follow(){
    std::vector<bool> IsNonterminalsNullable = IsNullableNon();
    std::vector<std::set<tokens> > follow;
    //follow.reserve(EF); //init vector to the size of non_terminals
    std::set<tokens> first_set;
    for (int i=0;i<NONTERMINAL_ENUM_SIZE;i++){
        follow.insert(follow.begin()+i, std::set<tokens >());
    }
    follow[S].insert(EF);
   /* first_set.insert(EF); //EF is representing $
    follow.insert(follow.begin(),first_set);
*/
   // follow.insert(follow.begin(),Set);
    //follow.insert(EF); //EF is representing $
    bool done = false;
    int nonterminal = S;
    bool nothing_changed = true;
    int index_rhs = 0;
    while(!done){

        if (nonterminal == NONTERMINAL_ENUM_SIZE){
            if (nothing_changed){
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
        //for each rule
        for (std::vector<grammar_rule>::const_iterator it = grammar.begin(); it != grammar.end(); it++) {
            //scan the right hand side of the rule & find the current non terminal
            index_rhs = 0;
            for(std::vector<int>::const_iterator rhs_it = (*it).rhs.begin(); rhs_it!=(*it).rhs.end(); rhs_it++) {
                //meant for checking if nothing_changed should be updated (post change)
                int current_follow_size = follow[nonterminal].size();
               /* if((*it).rhs[index_rhs] != nonterminal){
                //if(rhs_it[index_rhs] != nonterminal){
                    index_rhs++;
                    continue;
                }*/

                if((int)(*rhs_it) == nonterminal) {
                    //compute first set of clause
                    std::set<tokens> first;
                    std::vector<int> clause;
                    int index = index_rhs + 1;
                    int index_clause = 0;
                    /*  for(int i=0;i<(*it).rhs.size()-index;i++){
                          clause[i]=0;
                      }*/
                   // printf("\nsize of rhs = %d\n", (*it).rhs.size());
                    while (index < (*it).rhs.size()) {
                        clause.push_back((*it).rhs[index]);
                        index++;
                        index_clause++;
                    }
                    first = ClauseFirst(clause);

                    std::set_union(follow[nonterminal].begin(), follow[nonterminal].end(), first.begin(),
                                   first.end(), std::inserter(follow[nonterminal], follow[nonterminal].begin()));

                    //IsNull does not work here :( I added this lines who does the same. shani.
                    bool ClauseIsNull= true;
                    for(int var=0; var<clause.size();var++){
                        if(clause[var]>NONTERMINAL_ENUM_SIZE || !IsNonterminalsNullable[clause[var]]){
                            ClauseIsNull = false;
                            break;
                        }
                    }

                    if(ClauseIsNull){
                        std::set<tokens> follow_lhs;
                        follow_lhs = follow[(*it).lhs];
                        std::set_union(follow[nonterminal].begin(), follow[nonterminal].end(), follow_lhs.begin(),
                                       follow_lhs.end(),
                                       std::inserter(follow[nonterminal], follow[nonterminal].begin()));
                    }

                }
                else{
                    index_rhs++;
                }
                    int new_follow_size = follow[nonterminal].size();

                    if (new_follow_size != current_follow_size)
                        nothing_changed = false;



            }
        }
     //  std:cout << "curr non term = " << nonterminal << std::endl;
        nonterminal++;
    }
 //  print_follow(follow);
    return follow;

}

//TODO: add ducomantation
std::vector<std::set<tokens> > Select(){
    std::vector<bool> IsNonterminalsNullable = IsNullableNon();
    std::vector<std::set<tokens> > vec;
    //vec.reserve(grammar.size());
    for(int k=0;k<grammar.size();k++){
        vec.insert(vec.begin()+k,std::set<tokens>());
    }
    std::vector<std::set<tokens> > NonTermsFollow;
    for(int j=0;j<NONTERMINAL_ENUM_SIZE;j++){
        NonTermsFollow.insert(NonTermsFollow.begin()+j,std::set<tokens>());
    }
    NonTermsFollow = Follow(); //a vector contains all the follow sets of the non terminals

    int i =0; //rules counter
    for (std::vector<grammar_rule>::const_iterator it = grammar.begin(); it != grammar.end(); it++) {
        std::set<tokens > first_set_rhs = ClauseFirst((*it).rhs);

        //IsNull does not work here :( I added this lines who does the same. shani.
        bool ClauseIsNull= true;
        for(int var=0; var<(*it).rhs.size();var++){
            if((*it).rhs[var]>NONTERMINAL_ENUM_SIZE || !IsNonterminalsNullable[(*it).rhs[var]]){
                ClauseIsNull = false;
                break;
            }
        }

        if (ClauseIsNull){
            std::set_union(first_set_rhs.begin(),first_set_rhs.end() ,
                           NonTermsFollow[(*it).lhs].begin(),NonTermsFollow[(*it).lhs].end(),
                           std::inserter(vec[i],vec[i].begin()));
        }
        else{
            vec[i] = first_set_rhs;
          //  vec.insert(vec.begin()+i,first_set_rhs);
        }
        i++;
    }
    return vec;
}

//TODO: add documantation
void computeTable(std::map<nonterminal,std::map<tokens,int> >& M){
   // std::cout << "Compute table" << std::endl;
    std::vector<std::set<tokens> > select = Select();
   // print_select(select);
    int rule =0; //rules counter
      //iterate the select vectors of all the rules
     for (std::vector<std::set<tokens> >::const_iterator Terms_set = select.begin(); Terms_set != select.end(); Terms_set++){
         //itarate all the nonterminals in the rule's select vector
         nonterminal NonTerm = grammar[rule].lhs;
         for(std::set<tokens>::const_iterator terminal = (*Terms_set).begin(); terminal!=(*Terms_set).end(); terminal++){
             M[NonTerm][*terminal]=rule;
            // std::cout<< M[NonTerm][*terminal] << std::endl;
            // std::cout << "Rule: " << rule << " nonterminal: "<< NonTerm << "terminal: "<< *terminal <<std::endl;
         }
         rule++;
     }
    // iterate all the rules
    //for (std::vector<grammar_rule>::const_iterator it = grammar.begin(); it != grammar.end(); it++) {
      //  std::cout << "rule:" << counter << std::endl;
        //std::set<tokens> terminals = select[counter]; // the "select" terminal set of the rule
        // for each terminal in the select, add mapping between the Non termonal in the lest section of the rule and the map of the terminal and the rule
        //for (std::set<tokens>::const_iterator Terms_it = terminals.begin(); Terms_it != terminals.end(); Terms_it++) {
          //  std::cout << "terminal:" <<*Terms_it << std::endl;
            //const std::pair<tokens,int> pair(*Terms_it,counter); // a pair: (terminal, rule number)
            //std::cout << "create pair: " <<counter  <<", " << *Terms_it << std::endl;
            //std::map<nonterminal,std::map<tokens,int> >::iterator temp = (M.find((*it).lhs));
            //if (temp != M.end()){
              //  std::cout << "nontermonal found:" << (*it).lhs << std::endl;
                //(*temp).second.insert(pair);
            //}
            //else{ //nonterminal does not have a place in the map yet
              //  std::cout << "nontermonal set:" << (*it).lhs << std::endl;
                //std::map<tokens,int> MapPair = std::map<tokens,int>(); // mapping the terminal to the rule
                //MapPair.insert(pair);
               // const std::map<tokens,int>& MapPairRef(MapPair); // a reference to the map  pair
                //const std::pair<nonterminal ,std::map<tokens,int> > ToAdd((*it).lhs,MapPair); // maaping the nonterminal to the map of the terminal and the rule number
                //M.insert(ToAdd);
           // }
        //counter++;
        //}
    //}
}

//TODO: add documantation
bool MATCH (int X, int t){
    return (X==t);
}

/** ======================================== GIVEN FUNCTION STARTS HERE =============================================**/

/**
 * determines which variables are nullable, i.e. can produce an empty word
 * calls print_nullable when finished
 */
void compute_nullable(){
    std:cout << "compute_nullable" << std::endl; //todo: delete. this is for debug
    std::vector<bool> IsNonterminalsNullable = IsNullableNon(); //answer vector
    //init answer vector to false
    print_nullable(IsNonterminalsNullable);
}

/**
 * computes first for all nonterminal (see nonterminal enum in grammar.h)
 * calls print_first when finished
 */
void compute_first(){
    std:cout << "compute_first" << std::endl; //todo: delete. this is for debug

    std::vector<std::set<tokens> > vec1 = First(); // vec1 contains terminals too
    std::vector<std::set<tokens> > vec;
   // vec.reserve(NONTERMINAL_ENUM_SIZE);
    for(int i=0; i<NONTERMINAL_ENUM_SIZE;i++){
        vec.insert(vec.begin()+i,std::set<tokens>());
    }
    for(int i=0; i<NONTERMINAL_ENUM_SIZE; i++)
        vec[i] = vec1[i];  //copy all the non terminals to vec
    print_first(vec);
}


/**
 * computes follow for all nonterminal (see nonterminal enum in grammar.h)
 * calls print_follow when finished
 */
void compute_follow(){
    std:cout << "compute_follow" << std::endl; //todo: delete. this is for debug
    std::vector<std::set<tokens> > vec = Follow();
    print_follow(vec);
}

/**
 * computes select for all grammar rules (see grammar global variable in grammar.h)
 * calls print_select when finished
 */
void compute_select(){
    std:cout << "compute_select" << std::endl; //todo: delete. this is for debug
    std::vector<std::set<tokens> > vec = Select();
    print_select(vec);
}
/**
 * implements an LL(1) parser for the grammar using yylex()
 */
void parser(){
    std::cout << "parser" << std::endl; //todo: delete. this is for debug
    std::vector<int> Q;
    //initialization (rach)
    Q.push_back(nonterminal(S));
    std::map<nonterminal, std::map<tokens, int> > M;
    computeTable(M);
    //std::cout<<M[(nonterminal)0].size()<<std::endl;
   // for(int j=0; j<M.size(); j++){
      //  for(int t=0; t<M[(nonterminal)j].size(); t++){
        //    std::cout<<M[(nonterminal)j].size()<<std::endl;
            //std::cout << M[(nonterminal)j][tokens(t)] << std::endl;
       // }
  //  }

    int X; // will contain Q.pop
    std::cout << "before" << std::endl; //todo: delete. this is for debug
    //int t = yylex(); // the next token
    int t = SECTION; //for debug
    std::cout << "after" << std::endl; //todo: delete. this is for debug
    //int t = SECTION; //for debug
    //int index_d=0;
    int ruleNum;


    while(true){
        //TODO: fix yylex
        if(Q.empty()) {
            std::cout << "empty queue" << std::endl; //todo: delete. this is for debug

            // printf("\nhi\n");
            if (t == EF) {
                std::cout << "Success" << std::endl;
                return;
            } // $ - end of input
            else {
                std::cout << "Syntax error" << std::endl;
                return;
            }
        }
        else
            X = (Q.front());

        if (X > 10)
            MATCH (X,t);
        else{
            std::cout << "predict" << std::endl; //todo: delete. this is for debug
            std::cout << t << std::endl;
            std::cout << (nonterminal)X << std::endl;
            // PREDICT

            // try to find X in the map
            std::map<nonterminal ,std::map<tokens, int> >::iterator Find_Ans = M.find((nonterminal)X);
            if (Find_Ans == M.end()){
                //if X is not in the map
                std::cout << "Syntax error1" << std::endl; return; //todo: delete1

            }
            else {// X is in the map
                //try to find t in the X map
                std::map<tokens, int>::iterator Find_Ans2 = ((*Find_Ans).second).find((tokens)t);
                if (Find_Ans2 == ((*Find_Ans).second).end()){
                    // if t is not in the X map
                    std::cout << "Syntax error2" << std::endl; return; //todo: delete2

                }
                else{ // t is in the X map
                    ruleNum=(*Find_Ans2).second;
                }
            }



             //std::map<nonterminal,std::map<tokens,int> >::iterator mapTemp = (M.find((nonterminal)X));
            //if (mapTemp != M.end()){
             //   std::map<tokens,int>::iterator TokTemp = (*mapTemp).second.find((tokens)t);
               // if (TokTemp != (*mapTemp).second.end()){
                 //   ruleNum = (*TokTemp).second;
                //}
                //else{
                 //   std::cout << "Syntax error" << std::endl; exit(0);
                //}
           // }
            //else{
                //std::cout << "Syntax error" << std::endl; exit(0);
            }
            std::cout << ruleNum+1 << std::endl; //print the number of rule that was used
            Q.pop_back(); // get X out of the stack
            grammar_rule Rule = grammar[ruleNum];
            std::vector<int> ToPush = Rule.rhs;
            for(std::vector<int>::reverse_iterator var = ToPush.rbegin(); var != ToPush.rend(); ++var){
                // push the right side of the rule from end to begin
                Q.push_back(*var);
            }
        //t = yylex(); // fetch the next token
        t = EF; // for debug
    }

}


/*
 * implemented in lex.yy.c (generated from lexer.lex)
 */
int yylex();
