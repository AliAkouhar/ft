// #include "../inc/Server.hpp"
// #include "../inc/Channel.hpp"
// #include "../inc/Replies.hpp"
// #include "../inc/Client.hpp"

// std::vector<std::string> _split_buffer(const std::string& buffer,
// 											   const std::string& delimiter)
// {
// 	std::vector<std::string> tokens;
//     std::string::size_type prev_pos = 0;
//     std::string::size_type pos = buffer.find(delimiter);

//     if (pos != std::string::npos) {
//         tokens.push_back(buffer.substr(prev_pos, pos - prev_pos));
//         prev_pos = pos + 1;
//     }
//     tokens.push_back(buffer.substr(prev_pos, std::string::npos)); // Add the last token
//     return tokens;
// }

// #include <string>
// #include <vector>

// std::vector<std::string> _split_string(const std::string& s, char delimiter) {
//     std::vector<std::string> tokens;
//     size_t start = 0;
//     size_t end = s.find(delimiter);

//     while (end != std::string::npos) {
//         tokens.push_back(s.substr(start, end - start));
//         start = end + 1;
//         end = s.find(delimiter, start);
//     }
//     tokens.push_back(s.substr(start, std::string::npos));
//     return tokens;
// }

// int main()
// {
//     std::string buff = "#general,@avito,#priv";
//     std::vector<std::string> splited = _split_string(buff, ',');

//     std::vector<std::string>::iterator it;
//     for (it = splited.begin(); it != splited.end(); ++it) {
//         std::cout << *it << std::endl;
//     }
// }

#include "../inc/Server.hpp"

int main(int ac, char **av)
{
    (void)ac;
    Server server(av);
    server.setup();
}
