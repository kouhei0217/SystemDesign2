import os

import pymysql.cursors
from dotenv import load_dotenv

load_dotenv()

connection = pymysql.connect(host="localhost",
                             user="orikami",
                             password=os.getenv("password"),
                             db="SystemDesign2",
                             charset="utf8mb4",
                             cursorclass=pymysql.cursors.DictCursor)


def fetchMenus():
    try:
        with connection.cursor() as cursor:
            sql = "SELECT menus.menu_id, menus.menu_name FROM menus NATURAL JOIN votes WHERE votes.default_flag = 1 AND votes.year = YEAR (CURRENT_TIMESTAMP) AND votes.month = MONTH (CURRENT_TIMESTAMP);"
            cursor.execute(sql)
    finally:
        connection.close()
    return cursor.fetchall()[0]


def voteMenu(menuID):
    try:
        with connection.cursor() as cursor:
            sql = "INSERT INTO votes (year, month, menu_id, default_flag) VALUES (YEAR (CURRENT_TIMESTAMP), MONTH (CURRENT_TIMESTAMP), %s, FALSE);"
            cursor.execute(sql, (menuID))
        connection.commit()
    finally:
        connection.close()
