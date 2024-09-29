/**
 * @file Lexer.hpp
 * @author SW
 * @version 0.0.1
 * @date 2024-09-28
 *
 * @copyright Copyright (c) 2024 SW
 */
#pragma once

#include "Token.hpp"

namespace WandeltCore
{
	class Lexer
	{
	public:
		Lexer(const std::string& source);

		void Lex();

		const std::vector<Token>& GetTokens() const { return m_Tokens; }

	private:
		// Check if we are at the end of the source file
		bool IsAtEnd() const { return m_Current >= m_Source.size() - 1; }

		// Check if a character is a digit or not
		bool IsDigit(char c) const { return c >= '0' && c <= '9'; }

		// Check if a character is a letter or not (a-z, A-Z, $). Includes $ for identifiers.
		bool IsAlpha(char c) const { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '$'; }

		// Check if next character matches the expected character. Consume the character if it matches.
		bool IsMatchingNext(char expected);

		// Advance and consume the current character.
		char Advance();

		// Look ahead at the current character. Does not consume the character.
		char LookAhead(u32 offset = 0) const;

		void ScanToken();

		void AddToken(TokenType type);
		void AddToken(TokenType type, const std::string& lexeme);

	private:
		std::string m_Source; // The source code

		u32 m_Start   = 0; // The start of the current lexeme
		u32 m_Current = 0; // The current character index
		u32 m_Line    = 1; // The current line

		std::vector<Token> m_Tokens; // The lexed tokens
	};
} // namespace WandeltCore
