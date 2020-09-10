/**
 * @file      puzzle_db.cpp
 * @brief     
 * @details   
 * @author    R_W
 * @version     
 * @date      2020/7/20 1:36:21:784
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
 *         1.Date         -- 2020/7/20 1:36:21:784
 *           Author       -- R_W
 *           Modification -- Created file
 *
 */

#define  PUZZLE_DB_GLOBAL

/* includes-------------------------------------------------------------------*/
#include <QDebug>
#include <QDateTime>
#include "db/puzzle_db.hh"


    
/** @defgroup PUZZLE_DB                                            
  * @brief PUZZLE_DB system modules                                
  * @{                                                                         
  */
    
/* Private typedef&macro&definde----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* External functions --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
                                                                                
/** @defgroup PUZZLE_DB_Private_Functions                          
  * @{                                                                         
  */                                                                            
                                                                                
/**                                                                             
  * @}                                                                         
  */	                                                                        
                                                                                
/** @defgroup PUZZLE_DB_Global_Functions                          
  * @{                                                                         
  */   

bool Add_Puzzle(QString content, int level, QString& error){
    QSqlQuery q;
    QString sql = QString("insert into tb_puzzle(level, content, solved) \
                           values(%1, '%2', 0)").arg(level).arg(content);
    qDebug() << "sql:" << sql;
    if (!q.exec(sql)){
        error = q.lastError().text();
        return false;
    }
    return true;
}

std::shared_ptr<ST_PuzzleInfo_Type> Get_OnePuzzle_Unsolved(int level, QString& error){
    QSqlQuery query;
    QString sql = QString("SELECT id,content FROM tb_puzzle where level=%1 and solved=0 LIMIT 1;").arg(level);
    qDebug() << "sql:" << sql;
    if(query.exec(sql)){
        if (query.next()) {
            std::shared_ptr<ST_PuzzleInfo_Type> pInfo = std::make_shared<ST_PuzzleInfo_Type>();
            int fieldNo = query.record().indexOf("id");
            pInfo->id = query.value(fieldNo).toLongLong();
            pInfo->level = level;
            fieldNo = query.record().indexOf("content");
            pInfo->content = query.value(fieldNo).toString();
            pInfo->solved = false;
            return pInfo;            
        }
        qDebug() << "Get_OnePuzzle_Unsolved error:" << query.lastError().text();
        error = QString("谜题不存在");
        return nullptr;            
    }
    qDebug() << "Get_OnePuzzle_Unsolved error:" << query.lastError().text();
    error = query.lastError().text();
    return nullptr;      
}

bool Puzzle_Solved(uint64_t id, QString& error){
    QSqlQuery q;
    QString sql = QString("UPDATE tb_puzzle SET  solved=1 WHERE id=%1").arg(id);
    qDebug() << "sql:" << sql;
    if (!q.exec(sql)){
        error = q.lastError().text();
        return false;
    }
    return true;
}
                                                                                
/**                                                                             
  * @}                                                                         
  */	                                                                        
                                                                                
/**                                                                             
  * @}                                                                         
  */
                                                                                
/*************** (C) COPYRIGHT 2010-2018 R_W *********END OF FILE***********/

