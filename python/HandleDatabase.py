import os

import pymysql.cursors
from dotenv import load_dotenv

load_dotenv()


def StartConnection():
    return pymysql.connect(host="localhost",
                           user="orikami",
                           password=os.getenv("PASSWORD"),
                           db="SystemDesign2",
                           charset="utf8mb4",
                           cursorclass=pymysql.cursors.DictCursor)


def FetchImage():
    connection = StartConnection()
    try:
        with connection.cursor() as cursor:
            sql = "SELECT image_id FROM images WHERE image_id = (SELECT MAX(image_id) FROM images);"
            cursor.execute(sql)
            result = cursor.fetchall()
    finally:
        connection.close()
    return result


def SaveImage():
    connection = StartConnection()
    try:
        with connection.cursor() as cursor:
            sql = "INSERT INTO images(date) VALUES(CURRENT_DATE);"
            cursor.execute(sql)
        connection.commit()
    finally:
        connection.close()


def FetchMenu():
    connection = StartConnection()
    try:
        with connection.cursor() as cursor:
            sql = "SELECT menus.menu_name FROM menus NATURAL JOIN votes WHERE DATE_FORMAT(votes.date, '%Y%m') = DATE_FORMAT(DATE_ADD(CURRENT_DATE, INTERVAL 0 MONTH), '%Y%m') GROUP BY menu_id ORDER BY COUNT(default_flag) DESC LIMIT 1;"
            cursor.execute(sql)
            result = cursor.fetchall()
    finally:
        connection.close()
    return result


def FetchMenus():
    connection = StartConnection()
    try:
        with connection.cursor() as cursor:
            sql = "SELECT menus.menu_id, menus.menu_name FROM menus NATURAL JOIN votes WHERE votes.default_flag = 1 AND DATE_FORMAT(votes.date, '%Y%m') = DATE_FORMAT(CURRENT_DATE, '%Y%m');"
            cursor.execute(sql)
            result = cursor.fetchall()
    finally:
        connection.close()
    return result


def VoteMenu(menuID):
    connection = StartConnection()
    try:
        with connection.cursor() as cursor:
            sql = "INSERT INTO votes(menu_id) VALUES(%s);"
            cursor.execute(sql, (menuID))
        connection.commit()
    finally:
        connection.close()


def AddMenu(menuName):
    connection = StartConnection()
    try:
        with connection.cursor() as cursor:
            sql = "INSERT INTO menus(menu_name) VALUES(%s);"
            cursor.execute(sql, (menuName))
            sql = "INSERT INTO votes(menu_id, default_flag) SELECT menu_id, TRUE FROM menus WHERE menu_name = %s;"
            cursor.execute(sql, (menuName))
        connection.commit()
    finally:
        connection.close()
