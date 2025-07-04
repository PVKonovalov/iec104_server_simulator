//
// Created by Pavel Konovalov on 26/09/2024.
// https://github.com/jwbuurlage/flags
//

#ifndef FLAGS_H
#define FLAGS_H

#include <algorithm>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace flags {
    using namespace std::string_literals;

    namespace detail {
        inline std::vector<std::string> split(std::string s, const std::string& delim) {
            std::vector<std::string> result;
            size_t pos = 0;
            while ((pos = s.find(delim)) != std::string::npos) {
                std::string token = s.substr(0, pos);
                result.push_back(token);
                s.erase(0, pos + delim.length());
            }
            result.push_back(s);
            return result;
        }

        enum class flag_type {
            optional,
            required,
            flag,
        };
    } // namespace detail

    class flags {
    public:
        flags(int argc, char **argv) : argc_(argc), argv_(argv) {
        }

        void info(const std::string &programName, const std::string &description) {
            program_name_ = programName;
            description_ = description;
        }

        bool passed(const std::string& flag) {
            flags_.emplace_back(flag, detail::flag_type::flag, ""s, ""s);
            return passed_(flag);
        }

        bool passed_desc(const std::string& flag, const std::string& desc) {
            flags_.emplace_back(flag, detail::flag_type::flag, ""s, desc);
            return passed_(flag);
        }

        std::string arg(const std::string& flag) {
            flags_.emplace_back(flag, detail::flag_type::required, ""s, ""s);
            return arg_(flag);
        }

        std::vector<std::string> args(const std::string& flag) {
            flags_.emplace_back(flag, detail::flag_type::optional, "[]", ""s);

            auto pos = std::find(argv_, argv_ + argc_, flag);
            std::vector<std::string> result;
            if (pos == argv_ + argc_ || pos + 1 == argv_ + argc_) {
                return result;
            }
            pos++;
            while (pos != argv_ + argc_ && *pos[0] != '-') {
                result.push_back(std::string(*pos));
                pos++;
            }

            return result;
        }

        template<typename T>
        T arg_as(std::string flag) {
            flags_.emplace_back(flag, detail::flag_type::required, ""s, ""s);

            auto value = std::stringstream(arg_(flag));
            T x = {};
            value >> x;
            return x;
        }

        template<typename T>
        T arg_as_desc(std::string flag, std::string desc) {
            flags_.emplace_back(flag, detail::flag_type::required, ""s, desc);

            auto value = std::stringstream(arg_(flag));
            T x = {};
            value >> x;
            return x;
        }

        template<typename T>
        std::vector<T> args_as(std::string flag) {
            flags_.emplace_back(flag, detail::flag_type::optional, "[]", ""s);

            auto parts = detail::split(arg_(flag), ",");
            std::vector<T> xs;
            for (auto part: parts) {
                auto value = std::stringstream(part);
                T x = {};
                value >> x;
                xs.push_back(x);
            }

            return xs;
        }

        template<typename T>
        T arg_as_or(std::string flag, T alt) {
            flags_.emplace_back(flag, detail::flag_type::optional,
                                std::to_string(alt));

            if (!passed_(flag)) {
                return alt;
            }
            auto value = std::stringstream(arg_(flag));
            T x = {};
            value >> x;
            return x;
        }

        std::string arg_or(std::string flag, std::string alt) {
            flags_.emplace_back(flag, detail::flag_type::optional, alt, ""s);

            if (!passed_(flag)) {
                return alt;
            }
            return arg_(flag);
        }

        bool required_arguments(const std::vector<std::string> &args) {
            for (auto &arg: args) {
                if (!passed_(arg)) {
                    return false;
                }
            }
            return true;
        }

        bool sane() {
            for (auto [flag, type, alt, desc]: flags_) {
                (void) alt;
                if (type == detail::flag_type::required && !passed_(flag)) {
                    return false;
                }
            }
            return true;
        }

        std::string usage() {
            auto flag_size = [](auto &xs) { return std::get<0>(xs).size(); };

            auto max_flag_size = flag_size(*std::max_element(
                flags_.begin(), flags_.end(), [&](auto &lhs, auto &rhs) {
                    return flag_size(lhs) < flag_size(rhs);
                }));

            auto output = std::stringstream("");

            if (program_name_.empty()) {
                program_name_ = argv_[0];
            }
            output << program_name_ << "\n";
            if (!description_.empty()) {
                output << description_ << "\n";
            }
            output << "\n";

            output << "USAGE: " << program_name_ << " [OPTIONS]\n\n";

            output << "OPTIONS:\n";
            for (auto [flag, type, alt, desc]: flags_) {
                output << "    " << flag;

                auto padding = max_flag_size - flag.size();
                output << std::string(padding, ' ');
                if (type == detail::flag_type::optional) {
                    output << "    DEFAULT: " << alt << "  " << desc;
                }
                if (type == detail::flag_type::flag) {
                    output << "    FLAG" << "  " << desc;
                }
                if (type == detail::flag_type::required) {
                    output << "          " << desc;
                }

                output << "\n";
            }
            return output.str();
        }

    private:
        bool passed_(const std::string& flag) {
            return std::find(argv_, argv_ + argc_, flag) != (argv_ + argc_);
        }

        std::string arg_(const std::string& flag) {
            auto pos = std::find(argv_, argv_ + argc_, flag);
            if (pos == argv_ + argc_ || pos + 1 == argv_ + argc_) {
                return "";
            }
            pos++;
            return {*pos};
        }

        int argc_;
        char **argv_;
        std::vector<std::tuple<std::string, detail::flag_type, std::string, std::string> > flags_;
        std::string program_name_;
        std::string description_;
    };
} // namespace flags
#endif //FLAGS_H
