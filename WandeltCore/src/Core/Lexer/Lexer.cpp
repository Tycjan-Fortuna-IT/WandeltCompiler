#include "Lexer.hpp"

namespace WandeltCore
{
	Lexer::Lexer(const std::string& source) : m_Source(source)
	{
	}

	void Lexer::Lex()
	{
		while (!IsAtEnd())
		{
			m_Start = m_Current;

			ScanToken();
		}

		AddToken(TokenType::END_OF_FILE);
	}

	char Lexer::Advance()
	{
		return m_Source[m_Current++];
	}

	char Lexer::LookAhead(u32 offset) const
	{
		return m_Source[m_Current + offset];
	}

	void Lexer::ScanToken()
	{
		char c = Advance();

		switch (c)
		{
		case ' ':
		case '\r':
		case '\t':
			// Ignore whitespace.
			break;
		case '\n':
			++m_Line;
			break;
		case ';':
			AddToken(TokenType::SEMICOLON);
			break;
		case '=':
			AddToken(TokenType::EQUALS);
			break;
		default:
			if (IsDigit(c))
			{
				while (IsDigit(LookAhead())) Advance();

				AddToken(TokenType::NUMBER, m_Source.substr(m_Start, m_Current - m_Start));

				break;
			}
			else if (IsAlpha(c))
			{
				while (IsAlpha(LookAhead()) || IsDigit(LookAhead())) Advance();

				std::string lexeme = m_Source.substr(m_Start, m_Current - m_Start);

				auto it = Keywords.find(lexeme);
				if (it != Keywords.end())
				{
					AddToken(it->second);
				}
				else
				{
					if (c != '$')
					{
						SYSTEM_ERROR(
						    "Variable names must start with a $ character. Missing $ before: {} at line: {} column: {}",
						    lexeme, m_Line, m_Current - m_Start - lexeme.size() + 1);
						break;
					}

					AddToken(TokenType::IDENTIFIER, lexeme);
				}

				break;
			}

			SYSTEM_ERROR("Unexpected character: {}", c);
			break;
		}
	}

	void Lexer::AddToken(TokenType type)
	{
		m_Tokens.push_back(Token{type, std::nullopt, m_Line, m_Start + 1});
	}

	void Lexer::AddToken(TokenType type, const std::string& lexeme)
	{
		m_Tokens.push_back(Token{type, lexeme, m_Line, m_Start + 1});
	}

	//
} // namespace WandeltCore
