# CRUD (Pixar Rater)

**Description:**  
This project implements a website for creating and viewing reviews of Pixar movies. Users can create accounts to rate and write reviews for their favorite Pixar films. Reviews, movies, and user data are stored in a PostgreSQL database. The project uses **React** for the frontend and **Python with Flask** for the backend. I worked on this project with a classmate, and I was primarily responsible for the backend.


**Features:**  
- Users can **register**, **log in**, and manage their account.  
- Users can **add**, **edit**, and **delete reviews** for Pixar movies.  
- Displays movies along with their **average ratings** and user reviews.  
- Uses **PostgreSQL** to store movies, reviews, and user data.  
- Backend implemented in **Python with Flask**, using **psycopg2** for database interaction.  
- Frontend implemented in **React** with a responsive and interactive interface.
  
**Run Instructions:**
  
1. **Create and activate a virtual environment (recommended)**
   ```bash
   python -m venv venv
   source venv/bin/activate        # On macOS/Linux  
   venv\Scripts\activate           # On Windows
2. **Create .env**
   ```bash
   cd path/to/CRUD/web-app/api
   # Create a .env file with the following content:
   DB_HOST = 
   DB_NAME = 
   DB_USER = 
   DB_PASSWORD = 
   DB_PORT = 
   SECRET_KEY =

3. **Load the database**
   ```bash
   # Run the init.sql script in your PostgreSQL database

4. **Install dependencies**
   ```bash
   pip install -r requirements.txt

5. **Run the backend**:
   ```bash
   flask run
6. **Run the frontend (in a seperate terminal)**:
   ```bash
   cd path/to/CRUD/web-app
   npm install
   npm run


**Author:** 
Joakim Wiksten (and classmate)
