import psycopg2
from psycopg2 import OperationalError
import bcrypt
from dotenv import load_dotenv
import os

#Inserts a review into the database
def insert_review(movie_id:int, user_id:int, rating:float, review:str ):
    cursor.execute("""INSERT INTO reviews (movie_id, user_id, rating, review)
                   VALUES(%s,%s,%s,%s)""", (movie_id, user_id, rating, review))
    conn.commit()

#Removes a review from the database
def remove_review(review_id:int):
    cursor.execute("DELETE FROM reviews WHERE id = %s", (review_id,))
    conn.commit()

#Updates a review in the database
def update_review(rev_id:int, rating:float = None, review:str = None):
    if rating is not None and review is not None:
        cursor.execute("UPDATE reviews SET rating = %s, review = %s, updated_at = CURRENT_TIMESTAMP WHERE id = %s", (rating,review,rev_id))
    elif rating is not None:
        cursor.execute("UPDATE reviews SET rating = %s, updated_at = CURRENT_TIMESTAMP WHERE id = %s", (rating,rev_id))
    else:
        cursor.execute("UPDATE reviews SET review = %s, updated_at = CURRENT_TIMESTAMP WHERE id = %s", (review,rev_id))
    conn.commit()

#Returns the username of the user with specified email
def get_username(email:str):
    cursor.execute("SELECT username FROM users WHERE email = %s",(email,))
    result = cursor.fetchone()
    return result [0]

#Returns the user with specified email
def get_user(email:str):
    cursor.execute("SELECT * FROM users WHERE email = %s",(email,))
    return cursor.fetchone()

#Returns the average rating of a movie
def get_avg_rating(movie_id):
    cursor.execute("SELECT AVG(rating) FROM reviews WHERE movie_id = %s", (movie_id,))
    result = cursor.fetchone()
    return result[0]

#Returns all reviews for a movie in a specific order
def get_reviews_from_movie(movie_id:int, order_by:str):
    q = f"""
        SELECT r.rating, r.id, r.review, TO_CHAR(r.updated_at, 'YYYY-MM-DD HH24:MI:SS') As last_updated, u.username 
        FROM reviews AS r 
        JOIN users AS u ON u.user_id = r.user_id 
        WHERE r.movie_id = %s
        ORDER BY {order_by}
    """
    cursor.execute(q, (movie_id,))
    return cursor.fetchall()

#Returns info about a movie
def get_movie_info(movie_id: int):
    cursor.execute("SELECT m.*, ROUND(AVG(r.rating),1) AS avg_review FROM movies as m LEFT JOIN reviews AS r ON r.movie_id = m.id WHERE m.id = %s GROUP BY m.id", (movie_id,))
    return cursor.fetchall()

#Returns info about all movies in the database
def get_db_movies():
    cursor.execute("SELECT m.*, ROUND(AVG(r.rating),1) AS avg_review FROM movies AS m LEFT JOIN reviews AS r ON r.movie_id = m.id GROUP BY m.id ORDER BY m.year DESC")
    return cursor.fetchall()

#Inserts a user into the database
def create_user(username:str, email:str, password:str):
    cursor.execute("SELECT username, email FROM users WHERE username = %s OR email = %s", (username,email ))
    result = cursor.fetchone()
    if result is not None:
        return False
    p= password.encode("utf-8")
    salt = bcrypt.gensalt()
    hashed_password = bcrypt.hashpw(p,salt)
    cursor.execute("""INSERT INTO users (username, email, password_hash)
                   VALUES(%s,%s,%s)""", (username, email, hashed_password))
    conn.commit()
    return True

#Checks if a user exists and if the provided password is correct (for login)
def check_user(email:str, password:str):
    cursor.execute("SELECT password_hash FROM users WHERE email = %s",(email,))
    result = cursor.fetchone()
    if result is None:
        return False
    hashed_pword = result[0]
    p = password.encode("utf-8")
    return bcrypt.checkpw(p,bytes(hashed_pword))
    

#Load environment variables
load_dotenv()

def connect_to_db():
    try:
        conn = psycopg2.connect(
            dbname=os.getenv("DB_NAME"),
            user=os.getenv("DB_USER"),
            password=os.getenv("DB_PASSWORD"),
            host=os.getenv("DB_HOST"),
            port=os.getenv("DB_PORT")
        )
        print("✅ Connected successfully.")
        return conn

    except OperationalError as e:
        print("❌ Could not connect to the database.")
        print("Error details:", e)
        return None
conn = connect_to_db()
if conn:
    cursor = conn.cursor()
else:
    cursor = None