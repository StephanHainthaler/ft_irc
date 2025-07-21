/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shaintha <shaintha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 23:37:07 by shaintha          #+#    #+#             */
/*   Updated: 2025/07/09 23:37:07 by shaintha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <cstring> 

void	handleInput(void);
void    parseStringToVector(std::string &input, std::vector<std::string> *vector, const char *delimiters);
void	executeCommand(std::vector<std::string> command);
void    printVector(std::vector<std::string> vector);
size_t	kick(std::vector<std::string> command, size_t cmdNumber, std::string operatorName);

#endif
