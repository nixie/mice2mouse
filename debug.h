/*
 * File:    debug.h  (macro DEBUG used for preprocessor eliminable debugging)
 * Date:    21.10.2011 10:31
 * Author:  Radek Fér <xferra00@stud.fit.vutbr.cz>
 * Course:  ITU (user interfaces)
 */

#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifndef NDEBUG
#define DEBUG(x) x
#else
#define DEBUG(x)
#endif

#endif

