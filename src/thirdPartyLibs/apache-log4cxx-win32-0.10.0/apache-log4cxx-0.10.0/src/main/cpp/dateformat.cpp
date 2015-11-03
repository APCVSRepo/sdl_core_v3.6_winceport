/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifdef OS_WINCE
#include <stdlib.h>
#endif
#include <log4cxx/logstring.h>
#include <log4cxx/helpers/dateformat.h>
#include <log4cxx/helpers/stringhelper.h>


using namespace log4cxx;
using namespace log4cxx::helpers;

IMPLEMENT_LOG4CXX_OBJECT(DateFormat)

DateFormat::~DateFormat() {}

void DateFormat::setTimeZone(const TimeZonePtr&) {}

void DateFormat::numberFormat(LogString& s, int n, Pool& p) const {
    StringHelper::toString(n, p, s);
}

DateFormat::DateFormat() {}

