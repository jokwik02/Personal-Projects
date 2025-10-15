import React from "react";
import { BrowserRouter as Router, Routes, Route } from "react-router-dom";
import MoviesTable from "./components/MoviesTable";
import MovieDetails from "./components/MovieDetails";
import Header from "./components/Header";
import Login from "./components/Login";
import Register from "./components/Register";
import AuthProvider from "./context/AuthContext";
import ProtectedRoute from "./components/ProtectedRoute";


function App() {
  return (
	<AuthProvider>
    <Router>
	  <Header />
      <Routes>
        <Route path="/" element={<MoviesTable />} />
        <Route path="/movies/:id" element={<ProtectedRoute element={<MovieDetails />} />} />
		<Route path="/login" element={<Login />} />
        <Route path="/register" element={<Register />} />
      </Routes>
    </Router>
	</AuthProvider>
  );
}

export default App;
