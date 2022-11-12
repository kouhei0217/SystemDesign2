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

try:
    with connection.cursor() as cursor:
        sql = "INSERT INTO 'votes' ('month') VALUES (MONTH(CURRENT_TIMESTAMP), %d)"
        cursor.execute(sql, (1))

    connection.commit()

    # with connection.cursor() as cursor:
    #     sql = "SELECT `id`, `password` FROM `users` WHERE `email`=%s"
    #     cursor.execute(sql, ("webmaster@python.org",))
    #     result = cursor.fetchone()
    #     print(result)
finally:
    connection.close()
