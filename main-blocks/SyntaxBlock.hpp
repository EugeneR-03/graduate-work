#pragma once

#include <string>
#include <vector>
#include <boost/format.hpp>
#include <boost/signals2.hpp>
#include <boost/bind/bind.hpp>
#include <iostream>
#include <omp.h>

#include "./../base/Event.hpp"
#include "../tokens/Tokens.hpp"
#include "../messages/Messages.hpp"
#include "LexicalBlock.hpp"
#include "../repositories/SharedRepository.hpp"

using VariableToken = std::variant<SimpleToken, ComplexToken>;

enum class SyntaxBlockWorkingMode : short {
    UntilFirstError,
    AllErrors,
    AllErrorsInDetail,
};

inline std::string syntax_block_working_mode_to_string(SyntaxBlockWorkingMode errors_mode)
{
    switch (errors_mode) {
        case SyntaxBlockWorkingMode::UntilFirstError:
            return "UntilFirstError";
        case SyntaxBlockWorkingMode::AllErrors:
            return "AllErrors";
        case SyntaxBlockWorkingMode::AllErrorsInDetail:
            return "AllErrorsInDetail";
        default:
            return "Unknown";
    }
}

inline SyntaxBlockWorkingMode syntax_block_working_mode_from_string(const std::string& errors_mode)
{
    if (errors_mode == "UntilFirstError")
        return SyntaxBlockWorkingMode::UntilFirstError;
    else if (errors_mode == "AllErrors")
        return SyntaxBlockWorkingMode::AllErrors;
    else if (errors_mode == "AllErrorsInDetail")
        return SyntaxBlockWorkingMode::AllErrorsInDetail;
    else
        return SyntaxBlockWorkingMode::UntilFirstError;
}

class SyntaxBlock;

class CFGTemplate {
public:
    CFGTemplate(SyntaxBlock* syntax_block);
    virtual void cancel(bool is_sending_signal = true) = 0;
    virtual bool check(bool is_sending_signals = true) = 0;

protected:
    SyntaxBlock* syntax_block;
    int beginning_index = 0;
};

class CFGTemplateInner {
public:
    CFGTemplateInner(SyntaxBlock* syntax_block);
    virtual void cancel(bool is_sending_signal = true) = 0;
    virtual bool check(bool is_sending_signals = true, bool is_checking_inner = true) = 0;

protected:
    SyntaxBlock* syntax_block;
    int beginning_index = 0;
};

class SyntaxBlock {
private:
    SyntaxBlockWorkingMode working_mode;
    std::vector<VariableToken> token_vector;
    int current_token_index_in_vector;
    int string_index;

public:
    SyntaxBlock(SyntaxBlockWorkingMode working_mode = SyntaxBlockWorkingMode::UntilFirstError);
    ~SyntaxBlock();
    SyntaxBlockWorkingMode get_working_mode();
    void set_working_mode(SyntaxBlockWorkingMode working_mode);
    int get_string_index();
    void set_string_index(int index);
    void load_token_vector(const std::vector<VariableToken>& token_vector);
    void load_token();
    VariableToken get_current_token();
    int get_current_token_index();
    void cancel_load_token();
    Message make_message(const VariableToken& token, const std::string& message_pool, const std::string& message_pool_identifier);
    bool check_token_vector(const std::vector<std::variant<SimpleToken, ComplexToken>>& combined_tokens, SyntaxBlockWorkingMode working_mode);

    class CFG {
    public:
        class Symbol {
        public:
            class OpeningCurlyBrace : public CFGTemplate {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplate::CFGTemplate;
                bool check(bool is_sending_signals = true) override;
                void cancel(bool is_sending_signal = true) override;
            };

            class ClosingCurlyBrace : public CFGTemplate {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplate::CFGTemplate;
                bool check(bool is_sending_signals = true) override;
                void cancel(bool is_sending_signal = true) override;
            };

            class OpeningParenthesis : public CFGTemplate {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplate::CFGTemplate;
                bool check(bool is_sending_signals = true) override;
                void cancel(bool is_sending_signal = true) override;
            };

            class ClosingParenthesis : public CFGTemplate {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplate::CFGTemplate;
                bool check(bool is_sending_signals = true) override;
                void cancel(bool is_sending_signal = true) override;
            };

            class Comma : public CFGTemplate {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplate::CFGTemplate;
                bool check(bool is_sending_signals = true) override;
                void cancel(bool is_sending_signal = true) override;
            };

            class Colon : public CFGTemplate {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplate::CFGTemplate;
                bool check(bool is_sending_signals = true) override;
                void cancel(bool is_sending_signal = true) override;
            };

            class Semicolon : public CFGTemplate {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplate::CFGTemplate;
                bool check(bool is_sending_signals = true) override;
                void cancel(bool is_sending_signal = true) override;
            };

            class QuotationMark : public CFGTemplate {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplate::CFGTemplate;
                bool check(bool is_sending_signals = true) override;
                void cancel(bool is_sending_signal = true) override;
            };

            class EqualSign : public CFGTemplate {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplate::CFGTemplate;
                bool check(bool is_sending_signals = true) override;
                void cancel(bool is_sending_signal = true) override;
            };

            class ComparisonSign : public CFGTemplate {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplate::CFGTemplate;
                bool check(bool is_sending_signals = true) override;
                void cancel(bool is_sending_signal = true) override;
            };

            class ExclamationMark : public CFGTemplate {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplate::CFGTemplate;
                bool check(bool is_sending_signals = true) override;
                void cancel(bool is_sending_signal = true) override;
            };

            class ArithmeticSign : public CFGTemplate {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplate::CFGTemplate;
                bool check(bool is_sending_signals = true) override;
                void cancel(bool is_sending_signal = true) override;
            };

            class LogicalSign : public CFGTemplate {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplate::CFGTemplate;
                bool check(bool is_sending_signals = true) override;
                void cancel(bool is_sending_signal = true) override;
            };
        };

        class SpecialIdentifier {
        public:
            class Operation : public CFGTemplate {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplate::CFGTemplate;
                bool check(bool is_sending_signals = true) override;
                void cancel(bool is_sending_signal = true) override;
            };

            class OperandOfUnaryOperation : public CFGTemplate {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplate::CFGTemplate;
                bool check(bool is_sending_signals = true) override;
                void cancel(bool is_sending_signal = true) override;
            };

            class FirstOperandOfBinaryOperation : public CFGTemplate {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplate::CFGTemplate;
                bool check(bool is_sending_signals = true) override;
                void cancel(bool is_sending_signal = true) override;
            };

            class SecondOperandOfBinaryOperation : public CFGTemplate {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplate::CFGTemplate;
                bool check(bool is_sending_signals = true) override;
                void cancel(bool is_sending_signal = true) override;
            };

            class V : public CFGTemplate {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplate::CFGTemplate;
                bool check(bool is_sending_signals = true) override;
                void cancel(bool is_sending_signal = true) override;
            };

            class No : public CFGTemplate {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplate::CFGTemplate;
                bool check(bool is_sending_signals = true) override;
                void cancel(bool is_sending_signal = true) override;
            };

            class Real : public CFGTemplate {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplate::CFGTemplate;
                bool check(bool is_sending_signals = true) override;
                void cancel(bool is_sending_signal = true) override;
            };

            class Solution : public CFGTemplate {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplate::CFGTemplate;
                bool check(bool is_sending_signals = true) override;
                void cancel(bool is_sending_signal = true) override;
            };

            class Modulus : public CFGTemplate {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplate::CFGTemplate;
                bool check(bool is_sending_signals = true) override;
                void cancel(bool is_sending_signal = true) override;
            };

            class SquareOfNumber : public CFGTemplate {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplate::CFGTemplate;
                bool check(bool is_sending_signals = true) override;
                void cancel(bool is_sending_signal = true) override;
            };

            class SquareRootOfNumber : public CFGTemplate {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplate::CFGTemplate;
                bool check(bool is_sending_signals = true) override;
                void cancel(bool is_sending_signal = true) override;
            };
        };

        class Variable : public CFGTemplate {
        public:
            static inline Event<void (const Message&)> on_error_occurs;
            using CFGTemplate::CFGTemplate;
            bool check(bool is_sending_signals = true) override;
            void cancel(bool is_sending_signal = true) override;

            class Edge : public CFGTemplate {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplate::CFGTemplate;
                bool check(bool is_sending_signals = true) override;
                void cancel(bool is_sending_signal = true) override;
            };

            class Identifier : public CFGTemplate {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplate::CFGTemplate;
                bool check(bool is_sending_signals = true) override;
                void cancel(bool is_sending_signal = true) override;
            };

            class Integer : public CFGTemplate {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplate::CFGTemplate;
                bool check(bool is_sending_signals = true) override;
                void cancel(bool is_sending_signal = true) override;
            };
        };

        class Operation : public CFGTemplate {
        public:
            static inline Event<void (const Message&)> on_error_occurs;
            using CFGTemplate::CFGTemplate;
            bool check(bool is_sending_signals = true) override;
            void cancel(bool is_sending_signal = true) override;

            class Logical : public CFGTemplate {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplate::CFGTemplate;
                bool check(bool is_sending_signals = true) override;
                void cancel(bool is_sending_signal = true) override;
            };

            class Arithmetic : public CFGTemplate {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplate::CFGTemplate;
                bool check(bool is_sending_signals = true) override;
                void cancel(bool is_sending_signal = true) override;
            };
        };

        class String : public CFGTemplate {
        public:
            static inline Event<void (const Message&)> on_error_occurs;
            using CFGTemplate::CFGTemplate;
            bool check(bool is_sending_signals = true) override;
            void cancel(bool is_sending_signal = true) override;
            void check_all_inner_parts(bool is_sending_signals = true, bool is_checking_inner = false);

            class Beginning : public CFGTemplate {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplate::CFGTemplate;
                bool check(bool is_sending_signals = true) override;
                void cancel(bool is_sending_signal = true) override;
            };

            class Logical : public CFGTemplateInner {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplateInner::CFGTemplateInner;
                bool check(bool is_sending_signals = true, bool is_checking_inner = true) override;
                void cancel(bool is_sending_signal = true) override;
            };

            class Arithmetic : public CFGTemplateInner {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplateInner::CFGTemplateInner;
                bool check(bool is_sending_signals = true, bool is_checking_inner = true) override;
                void cancel(bool is_sending_signal = true) override;

                class NoRealSolution : public CFGTemplate {
                public:
                    static inline Event<void (const Message&)> on_error_occurs;
                    using CFGTemplate::CFGTemplate;
                    bool check(bool is_sending_signals = true) override;
                    void cancel(bool is_sending_signal = true) override;
                };
            };

            class Inner : public CFGTemplate {
            public:
                static inline Event<void (const Message&)> on_error_occurs;
                using CFGTemplate::CFGTemplate;
                bool check(bool is_sending_signals = true) override;
                void cancel(bool is_sending_signal = true) override;

                class Operation : public CFGTemplate {
                public:
                    static inline Event<void (const Message&)> on_error_occurs;
                    using CFGTemplate::CFGTemplate;
                    bool check(bool is_sending_signals = true) override;
                    void cancel(bool is_sending_signal = true) override;
                };

                class Operand : public CFGTemplateInner {
                public:
                    static inline Event<void (const Message&)> on_error_occurs;
                    using CFGTemplateInner::CFGTemplateInner;
                    bool check(bool is_sending_signals = true, bool is_checking_inner = true) override;
                    void cancel(bool is_sending_signal = true) override;

                    class Variable : public CFGTemplateInner {
                    public:
                        static inline Event<void (const Message&)> on_error_occurs;
                        using CFGTemplateInner::CFGTemplateInner;
                        bool check(bool is_sending_signals = true, bool is_checking_inner = true) override;
                        void cancel(bool is_sending_signal = true) override;
                    };

                    class Unary : public CFGTemplateInner {
                    public:
                        static inline Event<void (const Message&)> on_error_occurs;
                        using CFGTemplateInner::CFGTemplateInner;
                        bool check(bool is_sending_signals = true, bool is_checking_inner = true) override;
                        void cancel(bool is_sending_signal = true) override;
                    };

                    class Binary : public CFGTemplateInner {
                    public:
                        static inline Event<void (const Message&)> on_error_occurs;
                        using CFGTemplateInner::CFGTemplateInner;
                        bool check(bool is_sending_signals = true, bool is_checking_inner = true) override;
                        void cancel(bool is_sending_signal = true) override;

                        class First : public CFGTemplateInner {
                        public:
                            static inline Event<void (const Message&)> on_error_occurs;
                            using CFGTemplateInner::CFGTemplateInner;
                            bool check(bool is_sending_signals = true, bool is_checking_inner = true) override;
                            void cancel(bool is_sending_signal = true) override;
                        };

                        class Second : public CFGTemplateInner{
                        public:
                            static inline Event<void (const Message&)> on_error_occurs;
                            using CFGTemplateInner::CFGTemplateInner;
                            bool check(bool is_sending_signals = true, bool is_checking_inner = true) override;
                            void cancel(bool is_sending_signal = true) override;
                        };
                    };
                };
            };
        };
    };    
};