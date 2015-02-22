/*
 * Copyright 2013 Daniel Warner <contact@danrw.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include "Tle.h"

#include <locale>

namespace
{
    static const unsigned int TLE1_COL_NORADNUM = 2;
    static const unsigned int TLE1_LEN_NORADNUM = 5;
    static const unsigned int TLE1_COL_INTLDESC_A = 9;
    static const unsigned int TLE1_LEN_INTLDESC_A = 2;
    static const unsigned int TLE1_COL_INTLDESC_B = 11;
    static const unsigned int TLE1_LEN_INTLDESC_B = 3;
    static const unsigned int TLE1_COL_INTLDESC_C = 14;
    static const unsigned int TLE1_LEN_INTLDESC_C = 3;
    static const unsigned int TLE1_COL_EPOCH_A = 18;
    static const unsigned int TLE1_LEN_EPOCH_A = 2;
    static const unsigned int TLE1_COL_EPOCH_B = 20;
    static const unsigned int TLE1_LEN_EPOCH_B = 12;
    static const unsigned int TLE1_COL_MEANMOTIONDT2 = 33;
    static const unsigned int TLE1_LEN_MEANMOTIONDT2 = 10;
    static const unsigned int TLE1_COL_MEANMOTIONDDT6 = 44;
    static const unsigned int TLE1_LEN_MEANMOTIONDDT6 = 8;
    static const unsigned int TLE1_COL_BSTAR = 53;
    static const unsigned int TLE1_LEN_BSTAR = 8;
    static const unsigned int TLE1_COL_EPHEMTYPE = 62;
    static const unsigned int TLE1_LEN_EPHEMTYPE = 1;
    static const unsigned int TLE1_COL_ELNUM = 64;
    static const unsigned int TLE1_LEN_ELNUM = 4;

    static const unsigned int TLE2_COL_NORADNUM = 2;
    static const unsigned int TLE2_LEN_NORADNUM = 5;
    static const unsigned int TLE2_COL_INCLINATION = 8;
    static const unsigned int TLE2_LEN_INCLINATION = 8;
    static const unsigned int TLE2_COL_RAASCENDNODE = 17;
    static const unsigned int TLE2_LEN_RAASCENDNODE = 8;
    static const unsigned int TLE2_COL_ECCENTRICITY = 26;
    static const unsigned int TLE2_LEN_ECCENTRICITY = 7;
    static const unsigned int TLE2_COL_ARGPERIGEE = 34;
    static const unsigned int TLE2_LEN_ARGPERIGEE = 8;
    static const unsigned int TLE2_COL_MEANANOMALY = 43;
    static const unsigned int TLE2_LEN_MEANANOMALY = 8;
    static const unsigned int TLE2_COL_MEANMOTION = 52;
    static const unsigned int TLE2_LEN_MEANMOTION = 11;
    static const unsigned int TLE2_COL_REVATEPOCH = 63;
    static const unsigned int TLE2_LEN_REVATEPOCH = 5;
}

/**
 * Initialise the tle object.
 * @exception TleException
 */
void Tle::Initialize()
{
    if (!IsValidLineLength(line_one_))
    {
        std::string err = "Invalid length for line one:\n" + line_one_ + "\n";
        throw TleException(err.c_str());
    }

    if (!IsValidLineLength(line_two_))
    {
        std::string err = "Invalid length for line two:\n" + line_two_ + "\n";

        throw TleException(err.c_str());
    }

    if (line_one_[0] != '1')
    {
        throw TleException("Invalid line beginning for line one");
    }

    if (line_two_[0] != '2')
    {
        throw TleException("Invalid line beginning for line two");
    }

    unsigned long sat_number_1;
    unsigned long sat_number_2;

    ExtractULong(line_one_.substr(TLE1_COL_NORADNUM,
                TLE1_LEN_NORADNUM), sat_number_1);
    ExtractULong(line_two_.substr(TLE2_COL_NORADNUM,
                TLE2_LEN_NORADNUM), sat_number_2);

    if (sat_number_1 != sat_number_2)
    {
        throw TleException("Satellite numbers do not match");
    }

    norad_number_ = sat_number_1;

    if (name_.empty())
    {
        name_ = line_one_.substr(TLE1_COL_NORADNUM, TLE1_LEN_NORADNUM);
    }

    int_designator_ = line_one_.substr(TLE1_COL_INTLDESC_A,
            TLE1_LEN_INTLDESC_A + TLE1_LEN_INTLDESC_B + TLE1_LEN_INTLDESC_C);

    int year = 0;
    double day = 0.0;

    ExtractInteger(line_one_.substr(TLE1_COL_EPOCH_A,
                TLE1_LEN_EPOCH_A), year);
    ExtractDouble(line_one_.substr(TLE1_COL_EPOCH_B,
                TLE1_LEN_EPOCH_B), 4, day);
    ExtractDouble(line_one_.substr(TLE1_COL_MEANMOTIONDT2,
                TLE1_LEN_MEANMOTIONDT2), 2, mean_motion_dt2_);
    ExtractExponential(line_one_.substr(TLE1_COL_MEANMOTIONDDT6,
                TLE1_LEN_MEANMOTIONDDT6), mean_motion_ddt6_);
    ExtractExponential(line_one_.substr(TLE1_COL_BSTAR,
                TLE1_LEN_BSTAR), bstar_);

    /*
     * line 2
     */
    ExtractDouble(line_two_.substr(TLE2_COL_INCLINATION,
                TLE2_LEN_INCLINATION), 4, inclination_);
    ExtractDouble(line_two_.substr(TLE2_COL_RAASCENDNODE,
                TLE2_LEN_RAASCENDNODE), 4, right_ascending_node_);
    ExtractDouble(line_two_.substr(TLE2_COL_ECCENTRICITY,
                TLE2_LEN_ECCENTRICITY), -1, eccentricity_);
    ExtractDouble(line_two_.substr(TLE2_COL_ARGPERIGEE,
                TLE2_LEN_ARGPERIGEE), 4, argument_perigee_);
    ExtractDouble(line_two_.substr(TLE2_COL_MEANANOMALY,
                TLE2_LEN_MEANANOMALY), 4, mean_anomaly_);
    ExtractDouble(line_two_.substr(TLE2_COL_MEANMOTION,
                TLE2_LEN_MEANMOTION), 3, mean_motion_);
    ExtractULong(line_two_.substr(TLE2_COL_REVATEPOCH,
                TLE2_LEN_REVATEPOCH), orbit_number_);

    if (year < 57)
        year += 2000;
    else
        year += 1900;
    epoch_ = DateTime(year, day);
}

/**
 * Check
 * @param str The string to check
 * @returns Whether true of the string has a valid length
 */
bool Tle::IsValidLineLength(const std::string& str)
{
    return str.length() == LineLength() ? true : false;
}

/**
 * Convert a string containing an integer
 * @param[in] str The string to convert
 * @param[out] val The result
 * @exception TleException on conversion error
 */
void Tle::ExtractInteger(const std::string& str, int& val)
{
    std::string temp;
    bool found_digit = false;

    for (std::string::const_iterator i = str.begin(); i != str.end(); ++i)
    {
        if (isdigit(*i))
        {
            found_digit = true;
            temp += *i;
        }
        else if (found_digit)
        {
            throw TleException("Unexpected non digit");
        }
        else if (*i != ' ')
        {
            throw TleException("Invalid character");
        }
    }

    if (temp.length() == 0)
    {
        temp += '0';
    }

    if (!Util::FromString<int>(temp, val))
    {
        throw TleException("Failed to convert value to integer");
    }
}

/**
 * Convert a string containing an unsigned integer
 * @param[in] str The string to convert
 * @param[out] val The result
 * @exception TleException on conversion error
 */
void Tle::ExtractUInteger(const std::string& str, unsigned int& val)
{
    std::string temp;
    bool found_digit = false;

    for (std::string::const_iterator i = str.begin(); i != str.end(); ++i)
    {
        if (isdigit(*i))
        {
            found_digit = true;
            temp += *i;
        }
        else if (found_digit)
        {
            throw TleException("Unexpected non digit");
        }
        else if (*i != ' ')
        {
            throw TleException("Invalid character");
        }
    }

    if (temp.length() == 0)
    {
        temp += '0';
    }

    if (!Util::FromString<unsigned int>(temp, val))
    {
        throw TleException("Failed to convert value to integer");
    }
}

/**
 * Convert a string containing an unsigned long
 * @param[in] str The string to convert
 * @param[out] val The result
 * @exception TleException on conversion error
 */
void Tle::ExtractULong(const std::string& str, unsigned long& val)
{
    std::string temp;
    bool found_digit = false;

    for (std::string::const_iterator i = str.begin(); i != str.end(); ++i)
    {
        if (isdigit(*i))
        {
            found_digit = true;
            temp += *i;
        }
        else if (found_digit)
        {
            throw TleException("Unexpected non digit");
        }
        else if (*i != ' ')
        {
            throw TleException("Invalid character");
        }
    }

    if (temp.length() == 0)
    {
        temp += '0';
    }

    if (!Util::FromString<unsigned long>(temp, val))
    {
        throw TleException("Failed to convert value to integer");
    }
}

/**
 * Convert a string containing an double
 * @param[in] str The string to convert
 * @param[in] point_pos The position of the decimal point. (-1 if none)
 * @param[out] val The result
 * @exception TleException on conversion error
 */
void Tle::ExtractDouble(const std::string& str, int point_pos, double& val)
{
    std::string temp;
    bool found_digit = false;

    for (std::string::const_iterator i = str.begin(); i != str.end(); ++i)
    {
        /*
         * integer part
         */
        if (point_pos >= 0 && i < str.begin() + point_pos - 1)
        {
            bool done = false;

            if (i == str.begin())
            {
                if(*i == '-' || *i == '+')
                {
                    /*
                     * first character could be signed
                     */
                    temp += *i;
                    done = true;
                }
            }

            if (!done)
            {
                if (isdigit(*i))
                {
                    found_digit = true;
                    temp += *i;
                }
                else if (found_digit)
                {
                    throw TleException("Unexpected non digit");
                }
                else if (*i != ' ')
                {
                    throw TleException("Invalid character");
                }
            }
        }
        /*
         * decimal point
         */
        else if (point_pos >= 0 && i == str.begin() + point_pos - 1)
        {
            if (temp.length() == 0)
            {
                /*
                 * integer part is blank, so add a '0'
                 */
                temp += '0';
            }

            if (*i == '.')
            {
                /*
                 * decimal point found
                 */
                temp += *i;
            }
            else
            {
                throw TleException("Failed to find decimal point");
            }
        }
        /*
         * fraction part
         */
        else
        {
            if (i == str.begin() && point_pos == -1)
            {
                /*
                 * no decimal point expected, add 0. beginning
                 */
                temp += '0';
                temp += '.';
            }

            /*
             * should be a digit
             */
            if (isdigit(*i))
            {
                temp += *i;
            }
            else
            {
                throw TleException("Invalid digit");
            }
        }
    }

    if (!Util::FromString<double>(temp, val))
    {
        throw TleException("Failed to convert value to double");
    }
}

/**
 * Convert a string containing an exponential
 * @param[in] str The string to convert
 * @param[out] val The result
 * @exception TleException on conversion error
 */
void Tle::ExtractExponential(const std::string& str, double& val)
{
    std::string temp;

    for (std::string::const_iterator i = str.begin(); i != str.end(); ++i)
    {
        if (i == str.begin())
        {
            if (*i == '-' || *i == '+' || *i == ' ')
            {
                if (*i == '-')
                {
                    temp += *i;
                }
                temp += '0';
                temp += '.';
            }
            else
            {
                throw TleException("Invalid sign");
            }
        }
        else if (i == str.begin() + static_cast<std::ptrdiff_t>(str.length()) - 2)
        {
            if (*i == '-' || *i == '+')
            {
                temp += 'e';
                temp += *i;
            }
            else
            {
                throw TleException("Invalid exponential sign");
            }
        }
        else
        {
            if (isdigit(*i))
            {
                temp += *i;
            }
            else
            {
                throw TleException("Invalid digit");
            }
        }
    }

    if (!Util::FromString<double>(temp, val))
    {
        throw TleException("Failed to convert value to double");
    }
}
