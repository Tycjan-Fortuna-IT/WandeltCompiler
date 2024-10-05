#include "Lexer.hpp"

#include "Core/FileSystem/FileSystem.hpp"

#define TOKEN_CASE(x)                \
	AddToken(tokenStartLocation, x); \
	break;
namespace WandeltCore
{
	Lexer::Lexer(const std::filesystem::path& filepath) : m_Filename(filepath.filename().string())
	{
		ASSERT(FileSystem::Exists(filepath), "Input file does not exist.");

		SYSTEM_INFO("Input file: {}", filepath.filename());

		m_Source = FileSystem::ReadFile(filepath);
	}

	Lexer::Lexer(const std::string& filename, const std::string& source) : m_Source(source), m_Filename(filename)
	{
	}

	void Lexer::Lex()
	{
		while (!IsAtEnd() && m_IsValid)
		{
			m_Start = m_Current;

			ScanToken();
		}

		AddToken({m_Filename, m_Line, m_Column}, TokenType::END_OF_FILE);
	}

	bool Lexer::IsMatchingNext(char expected)
	{
		if (IsAtEnd())
			return false;

		if (m_Source[m_Current] != expected)
			return false;

		m_Current++;

		return true;
	}

	char Lexer::Advance()
	{
		++m_Column;

		if (m_Source[m_Current] == '\n')
		{
			++m_Line;
			m_Column = 0;
		}

		return m_Source[m_Current++];
	}

	char Lexer::LookAhead(u32 offset) const
	{
		return m_Source[m_Current + offset];
	}

	void Lexer::ScanToken()
	{
		char c = Advance();

		const SourceLocation tokenStartLocation = {m_Filename, m_Line, m_Column};

		switch (c)
		{
		case ' ':
		case '\r':
		case '\t':
		case '\n':
			// Ignore whitespace.
			break;
		case ';':
			TOKEN_CASE(TokenType::SEMICOLON);
		case '=':
			TOKEN_CASE(TokenType::EQUALS);
		case '(':
			TOKEN_CASE(TokenType::LEFT_PARENTHESES);
		case ')':
			TOKEN_CASE(TokenType::RIGHT_PARENTHESES);
		case '{':
			TOKEN_CASE(TokenType::LEFT_BRACE);
		case '}':
			TOKEN_CASE(TokenType::RIGHT_BRACE);
		case '+':
			TOKEN_CASE(TokenType::PLUS);
		case '-':
			TOKEN_CASE(TokenType::MINUS);
		case '*': {
			if (IsMatchingNext('*'))
			{
				TOKEN_CASE(TokenType::DOUBLE_STAR);
			}
			else
			{
				TOKEN_CASE(TokenType::STAR);
			}
		}
			TOKEN_CASE(TokenType::STAR);
		case '/': {
			if (IsMatchingNext('/'))
				// A comment goes until the end of the line.
				while (LookAhead() != '\n' && !IsAtEnd()) Advance();
			else if (IsMatchingNext('*'))
			{
				// A block comment goes until the end of the block.
				while (!(LookAhead() == '*' && LookAhead(1) == '/') && !IsAtEnd())
				{
					if (LookAhead() == '\n')
						++m_Line;

					Advance();
				}

				if (IsAtEnd())
				{
					SYSTEM_ERROR("Unterminated block comment at line: {} column: {}", m_Line, m_Current - m_Start);
					m_IsValid = false;
					break;
				}

				// Consume the '*/'
				if (!IsAtEnd())
				{
					Advance();
					Advance();
				}
			}
			else
				TOKEN_CASE(TokenType::SLASH);
		}
		case '%':
			TOKEN_CASE(TokenType::PERCENT);
		default:
			if (IsDigit(c))
			{
				while (IsDigit(LookAhead())) Advance();

				AddToken(tokenStartLocation, TokenType::NUMBER, m_Source.substr(m_Start, m_Current - m_Start));

				break;
			}
			else if (IsAlpha(c))
			{
				while (IsAlpha(LookAhead()) || IsDigit(LookAhead())) Advance();

				std::string lexeme = m_Source.substr(m_Start, m_Current - m_Start);

				auto it = Keywords.find(lexeme);
				if (it != Keywords.end())
				{
					AddToken(tokenStartLocation, it->second);
				}
				else
				{
					if (c != '$')
					{
						SYSTEM_ERROR(
						    "Variable names must start with a $ character. Missing $ before: {} at line: {} column: {}",
						    lexeme, tokenStartLocation.Line, tokenStartLocation.Column);
						break;
					}

					AddToken(tokenStartLocation, TokenType::IDENTIFIER, lexeme);
				}

				break;
			}

			SYSTEM_ERROR("Unexpected character: {} at line: {} column: {}. Skipping.", c, tokenStartLocation.Line,
			             tokenStartLocation.Column);

			m_IsValid = false;

			break;
		}
	}

	void Lexer::AddToken(const SourceLocation& location, TokenType type)
	{
		m_Tokens.push_back(Token{type, std::nullopt, location});
	}

	void Lexer::AddToken(const SourceLocation& location, TokenType type, const std::string& lexeme)
	{
		m_Tokens.push_back(Token{type, lexeme, location});
	}

	//
} // namespace WandeltCore
