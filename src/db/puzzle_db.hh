/**
 * @file      puzzle_db.hh
 * @brief     
 * @details   
 * @author    R_W
 * @version     
 * @date      2020/7/20 1:38:23:724
 * @copyright R_W
 * @par         (c) COPYRIGHT 2010-2018 by R_W Systems, Inc.
 *                        All rights reserved.
 *                                                                    
 *       This software is confidential and proprietary to R_W
 *     Systems, Inc.  No part of this software may be reproduced,    
 *     stored, transmitted, disclosed or used in any form or by any means
 *     other than as expressly provided by the written license agreement    
 *     between R_W Systems and its licensee.
 * @par History      
 *         1.Date         -- 2020/7/20 1:38:23:724
 *           Author       -- R_W
 *           Modification -- Created file
 *
 */
#ifndef __PUZZLE_DB_HH__
#define __PUZZLE_DB_HH__

#ifdef  PUZZLE_DB_GLOBAL
#define PUZZLE_DB_EXT
#else
#define PUZZLE_DB_EXT extern
#endif /* PUZZLE_DB_GLOBAL */

/*============================================================================*/
/*                                  @INCLUDES                                 */
/*============================================================================*/
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <memory>


/** @addtogroup PUZZLE_DB
  * @{
  */
 
/*============================================================================*/
/*                             @MACROS & @TYPEDEFS                            */
/*============================================================================*/
typedef struct{
    uint64_t id;
    int level;
    QString content;
    bool solved;
}ST_PuzzleInfo_Type;


                                                                                
/*============================================================================*/
/*                             @GLOBAL VIRIABLES                              */
/*============================================================================*/
                                                                                
/*============================================================================*/
/*                                   @FUNCS                                   */
/*============================================================================*/
bool Add_Puzzle(QString content, int level, QString& error);
std::shared_ptr<ST_PuzzleInfo_Type> Get_OnePuzzle_Unsolved(int level, QString& error);
bool Puzzle_Solved(uint64_t id, QString& error);



/**
  * @}
  */ 
		
#endif /* __PUZZLE_DB_HH__ */
/*************** (C) COPYRIGHT 2010-2018 R_W *********END OF FILE***********/

