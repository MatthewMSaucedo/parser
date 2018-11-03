#include "token.h"
#include "data.h"
#include "symbol.h"
#include <string.h>
#include <stdlib.h>

/**
 * This pointer is set when by parser() func and used by printParsedToken() func.
 * 
 * You are not required to use it anywhere. The implemented part of the skeleton
 * handles the printing. Instead, You could use the supplied helper functions to
 * manipulate the output file.
 * */
FILE* _out;

/**
 * Token list iterator used by the parser. It will be set once entered to parser()
 * and reset before exiting parser().
 * 
 * It is better to use the given helper functions to make use of token list iterator.
 * */
TokenListIterator _token_list_it;

/**
 * Current level.
 * */
unsigned int currentLevel;

/**
 * Symbol table.
 * */
SymbolTable symbolTable;

/**
 * Returns the current token using the token list iterator.
 * If it is the end of tokens, returns token with id nulsym.
 * */
Token getCurrentToken();

/**
 * Returns the type of the current token. Returns nulsym if it is the end of tokens.
 * */
int getCurrentTokenType();

/**
 * Prints the given token on _out by applying required formatting.
 * */
void printCurrentToken();

/**
 * Advances the position of TokenListIterator by incrementing the current token
 * index by one.
 * */
void nextToken();

/**
 * Given an entry from non-terminal enumaration, prints it.
 * */
void printNonTerminal(NonTerminal nonTerminal);

/**
 * Functions used for non-terminals of the grammar
 * */
int program();
int block();
int const_declaration();
int var_declaration();
int proc_declaration();
int statement();
int condition();
int relop();
int expression();
int term();
int factor();

Token getCurrentToken()
{
    return getCurrentTokenFromIterator(_token_list_it);
}

int getCurrentTokenType()
{
    return getCurrentToken().id;
}

void printCurrentToken()
{
    fprintf(_out, "%8s <%s, '%s'>\n", "TOKEN  :", tokenNames[getCurrentToken().id], getCurrentToken().lexeme);
}

void nextToken()
{
    _token_list_it.currentTokenInd++;
}

void printNonTerminal(NonTerminal nonTerminal)
{
    fprintf(_out, "%8s %s\n", "NONTERM:", nonTerminalNames[nonTerminal]);
}

/**
 * Given the parser error code, prints error message on file by applying
 * required formatting.
 * */
void printParserErr(int errCode, FILE* fp)
{
    if(!fp) return;

    if(!errCode)
        fprintf(fp, "\nPARSING WAS SUCCESSFUL.\n");
    else
        fprintf(fp, "\nPARSING ERROR[%d]: %s.\n", errCode, parserErrorMsg[errCode]);
}

/**
 * Advertised parser function. Given token list, which is possibly the output of 
 * the lexer, parses the tokens. If encountered, return the error code.
 * 
 * Returning 0 signals successful parsing.
 * Otherwise, returns a non-zero parser error code.
 * */
int parser(TokenList tokenList, FILE* out)
{
    // Set output file pointer
    _out = out;

    /**
     * Create a token list iterator, which helps to keep track of the current
     * token being parsed.
     * */
    _token_list_it = getTokenListIterator(&tokenList);

    // Initialize current level to 0, which is the global level
    currentLevel = 0;

    // Initialize symbol table
    initSymbolTable(&symbolTable);

    // Write parsing history header
    fprintf(_out, "Parsing History\n===============\n");

    // Start parsing by parsing program as the grammar suggests.
    int err = program();

    // Print symbol table - if no error occured
    if(!err)
    {
        fprintf(_out, "\n\n");
        printSymbolTable(&symbolTable, _out);
    }

    // Reset output file pointer
    _out = NULL;

    // Reset the global TokenListIterator
    _token_list_it.currentTokenInd = 0;
    _token_list_it.tokenList = NULL;

    // Delete symbol table
    deleteSymbolTable(&symbolTable);

    // Return err code - which is 0 if parsing was successful
    return err;
}

int program()
{
    printNonTerminal(PROGRAM);

    /**
     * Program is a block followed by "."
     * */
	 
	// Parse block.
    int err = block();

    /**
     * If parsing of block was not successful, immediately stop parsing
     * and propagate the same error code by returning it.
     * */
    if(err) return err;

    // After block, period should come
    if(getCurrentTokenType() != periodsym)
    {
        /**
         * Error code 6: Period expected.
         * Stop parsing and return error code 6.
         * */
        return 6;
    }

    // It was a periodsym. Consume periodsym.
    printCurrentToken(); // Printing the token is essential!
    nextToken(); // Go to the next token..
	
    return 0;
}

int block()
{
    printNonTerminal(BLOCK);

    /**
     * block is 
	 * 1) const_declaration
	 * 2) var_declaration
	 * 3) proc_declaration
	 * 4) statement
     * */
	 
	// Parse const_declaration.
    int err = const_declaration();

    /**
     * If parsing of const_declaration was not successful, immediately stop parsing
     * and propagate the same error code by returning it.
     * */
    if(err) return err;

    // Parse var_declaration.
    err = var_declaration();

    /**
     * If parsing of var_declaration was not successful, immediately stop parsing
     * and propagate the same error code by returning it.
     * */
    if(err) return err;
	
	// Parse proc_declaration.
    err = proc_declaration();

    /**
     * If parsing of proc_declaration was not successful, immediately stop parsing
     * and propagate the same error code by returning it.
     * */
    if(err) return err;
	
	// Parse statement.
    err = statement();

    /**
     * If parsing of statement was not successful, immediately stop parsing
     * and propagate the same error code by returning it.
     * */
    if(err) return err;

    return 0;
}

int const_declaration()
{
    printNonTerminal(CONST_DECLARATION);

    /**
     * const_declaration is the following, though this is optional 
	 * 1) "const"
	 * 2) ident
	 * 3) "="
	 * 4) number
	 *		repeat this 0 or more times
	 *		a) ","
	 *		b) ident
	 *		c) "="
	 *		d) number 
	 * 5) ";"
     * */
	
	// Is the current token a constsym?
    if (getCurrentTokenType() == constsym)
	{
		// Consume constsym
		printCurrentToken(); // Printing the token is essential!
		nextToken(); // Go to the next token..
		
		// Is the current token a identsym?
		if (getCurrentTokenType() == identsym)
		{
			// Consume identsym
			printCurrentToken(); // Printing the token is essential!
			nextToken(); // Go to the next token..
		}
		else
		{
			/**
             * Error code 3: 'const', 'var', 'procedure', 'read', 'write' must be followed by identifier.
             * Stop parsing and return error code 3.
             * */
            return 3;
		}
		
		// Is the current token a eqsym? 
		if (getCurrentTokenType() == eqsym)
		{
			// Consume eqsym
			printCurrentToken(); // Printing the token is essential!
			nextToken(); // Go to the next token..
		}
		else
		{
			/**
             * Error code 2: Identifier must be followed by '='.
             * Stop parsing and return error code 2.
             * */
            return 2;
		}
		
		// Is the current token a numbersym? 
		if (getCurrentTokenType() == numbersym)
		{
			// Consume numbersym
			printCurrentToken(); // Printing the token is essential!
			nextToken(); // Go to the next token..
		}
		else
		{
			/**
             * Error code 1: '=' must be followed by a number.
             * Stop parsing and return error code 1.
             * */
            return 1;
		}
		
		while (getCurrentTokenType() == commasym)
		{
			// Consume commasym
			printCurrentToken(); // Printing the token is essential!
			nextToken(); // Go to the next token..
			
			// Is the current token a identsym?
			if (getCurrentTokenType() == identsym)
			{
				// Consume identsym
				printCurrentToken(); // Printing the token is essential!
				nextToken(); // Go to the next token..
			}
			else
			{
				/**
				 * Error code 3: 'const', 'var', 'procedure', 'read', 'write' must be followed by identifier.
				 * Stop parsing and return error code 3.
				 * */
				return 3;
			}
			
			// Is the current token a eqsym? 
			if (getCurrentTokenType() == eqsym)
			{
				// Consume eqsym
				printCurrentToken(); // Printing the token is essential!
				nextToken(); // Go to the next token..
			}
			else
			{
				/**
				 * Error code 2: Identifier must be followed by '='.
				 * Stop parsing and return error code 2.
				 * */
				return 2;
			}
			
			// Is the current token a numbersym? 
			if (getCurrentTokenType() == numbersym)
			{
				// Consume numbersym
				printCurrentToken(); // Printing the token is essential!
				nextToken(); // Go to the next token..
			}
			else
			{
				/**
				 * Error code 1: '=' must be followed by a number.
				 * Stop parsing and return error code 1.
				 * */
				return 1;
			}
		}
		
		// Is the current token a semicolonsym? 
		if (getCurrentTokenType() == semicolonsym)
		{
			// Consume semicolonsym
			printCurrentToken(); // Printing the token is essential!
			nextToken(); // Go to the next token..
		}
		else
		{
			/**
             * Error code 4: Semicolon or comma missing.
             * Stop parsing and return error code 4.
             * */
            return 4;
		}
	}
	
    // Successful parsing.
    return 0;
}

int var_declaration()
{
    printNonTerminal(VAR_DECLARATION);

    // Is the current token a varsym?
    if (getCurrentTokenType() == varsym)
	{
		// Consume varsym
		printCurrentToken(); // Printing the token is essential!
		nextToken(); // Go to the next token..
		
		// Is the current token a identsym?
		if (getCurrentTokenType() == identsym)
		{
			// Consume identsym
			printCurrentToken(); // Printing the token is essential!
			nextToken(); // Go to the next token..
		}
		else
		{
			/**
             * Error code 3: 'const', 'var', 'procedure', 'read', 'write' must be followed by identifier.
             * Stop parsing and return error code 3.
             * */
            return 3;
		}
		
		while (getCurrentTokenType() == commasym)
		{
			// Consume commasym
			printCurrentToken(); // Printing the token is essential!
			nextToken(); // Go to the next token..
			
			// Is the current token a identsym?
			if (getCurrentTokenType() == identsym)
			{
				// Consume identsym
				printCurrentToken(); // Printing the token is essential!
				nextToken(); // Go to the next token..
			}
			else
			{
				/**
				 * Error code 3: 'const', 'var', 'procedure', 'read', 'write' must be followed by identifier.
				 * Stop parsing and return error code 3.
				 * */
				return 3;
			}
		}
		
		// Is the current token a semicolonsym? 
		if (getCurrentTokenType() == semicolonsym)
		{
			// Consume semicolonsym
			printCurrentToken(); // Printing the token is essential!
			nextToken(); // Go to the next token..
		}
		else
		{
			/**
             * Error code 4: Semicolon or comma missing.
             * Stop parsing and return error code 4.
             * */
            return 4;
		}
	}

    return 0;
}

int proc_declaration()
{
    printNonTerminal(PROC_DECLARATION);

    while (getCurrentTokenType() == procsym)
	{
		// Consume procsym
		printCurrentToken(); // Printing the token is essential!
		nextToken(); // Go to the next token..
			
		// Is the current token a identsym?
		if (getCurrentTokenType() == identsym)
		{
			// Consume identsym
			printCurrentToken(); // Printing the token is essential!
			nextToken(); // Go to the next token..
		}
		else
		{
			/**
			 * Error code 3: 'const', 'var', 'procedure', 'read', 'write' must be followed by identifier.
			 * Stop parsing and return error code 3.
			 * */
			return 3;
		}
		
		// Is the current token a semicolonsym? 
		if (getCurrentTokenType() == semicolonsym)
		{
			// Consume semicolonsym
			printCurrentToken(); // Printing the token is essential!
			nextToken(); // Go to the next token..
		}
		else
		{
			/**
             * Error code 5: Semicolon missing.
             * Stop parsing and return error code 5.
             * */
            return 5;
		}
		
		// Parse block.
		int err = block();

		/**
		* If parsing of block was not successful, immediately stop parsing
		* and propagate the same error code by returning it.
		* */
		if(err) return err;
		
		// Is the current token a semicolonsym? 
		if (getCurrentTokenType() == semicolonsym)
		{
			// Consume semicolonsym
			printCurrentToken(); // Printing the token is essential!
			nextToken(); // Go to the next token..
		}
		else
		{
			/**
             * Error code 5: Semicolon missing.
             * Stop parsing and return error code 5.
             * */
            return 5;
		}
	}

    return 0;
}

int statement()
{
    printNonTerminal(STATEMENT);

    /* TODO: Implement */

    return 0;
}

int condition()
{
    printNonTerminal(CONDITION);

    /* TODO: Implement */

    return 0;
}

int relop()
{
    printNonTerminal(REL_OP);

    /* TODO: Implement */

    return 0;
}

int expression()
{
    printNonTerminal(EXPRESSION);

    /* TODO: Implement */

    return 0;
}

int term()
{
    printNonTerminal(TERM);

    /* TODO: Implement */

    return 0;
}

/**
 * The below function is left fully-implemented as a hint.
 * */
int factor()
{
    printNonTerminal(FACTOR);

    /**
     * There are three possibilities for factor:
     * 1) ident
     * 2) number
     * 3) '(' expression ')'
     * */

    // Is the current token a identsym?
    if(getCurrentTokenType() == identsym)
    {
        // Consume identsym
        printCurrentToken(); // Printing the token is essential!
        nextToken(); // Go to the next token..

        // Success
        return 0;
    }
    // Is that a numbersym?
    else if(getCurrentTokenType() == numbersym)
    {
        // Consume numbersym
        printCurrentToken(); // Printing the token is essential!
        nextToken(); // Go to the next token..

        // Success
        return 0;
    }
    // Is that a lparentsym?
    else if(getCurrentTokenType() == lparentsym)
    {
        // Consume lparentsym
        printCurrentToken(); // Printing the token is essential!
        nextToken(); // Go to the next token..

        // Continue by parsing expression.
        int err = expression();

        /**
         * If parsing of expression was not successful, immediately stop parsing
         * and propagate the same error code by returning it.
         * */
        
        if(err) return err;

        // After expression, right-parenthesis should come
        if(getCurrentTokenType() != rparentsym)
        {
            /**
             * Error code 13: Right parenthesis missing.
             * Stop parsing and return error code 13.
             * */
            return 13;
        }

        // It was a rparentsym. Consume rparentsym.
        printCurrentToken(); // Printing the token is essential!
        nextToken(); // Go to the next token..
    }
    else
    {
        /**
          * Error code 24: The preceding factor cannot begin with this symbol.
          * Stop parsing and return error code 24.
          * */
        return 14;
    }

    return 0;
}











/*
do
{
			
} while();
*/