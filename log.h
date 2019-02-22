//
// Created by 23591 on 2019/1/24.
//

#ifndef MD5_DIR_LOG_H
#define MD5_DIR_LOG_H

#include<boost/log/trivial.hpp>

#define TRACE(msg) BOOST_LOG_TRIVIAL(trace)<<msg;
#define DEBUG(msg) BOOST_LOG_TRIVIAL(debug)<<msg;
#define INFO(msg) BOOST_LOG_TRIVIAL(info)<<msg;
#define WARN(msg) BOOST_LOG_TRIVIAL(warning)<<msg;
#define ERROR(msg) BOOST_LOG_TRIVIAL(error)<<msg;
#define FATAL(msg) BOOST_LOG_TRIVIAL(fatal)<<msg;

#endif //MD5_DIR_LOG_H
