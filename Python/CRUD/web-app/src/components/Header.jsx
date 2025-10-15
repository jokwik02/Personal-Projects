import React, { useContext } from "react";
import { AppBar, Toolbar, Typography, Button } from "@mui/material";
import { useNavigate } from "react-router-dom";
import { AuthContext } from "../context/AuthContext";

const Header = () => {
  const navigate = useNavigate();
  const { user, logout } = useContext(AuthContext);

  return (
    <AppBar position="static" sx={{ backgroundColor: "grey.800" }}>
      <Toolbar>
        {/* Web App Name - Click to navigate home */}
        <Typography variant="h6" component="div" sx={{ flexGrow: 1, cursor: "pointer" }} onClick={() => navigate("/")}>
          Pixar Rater
        </Typography>

        {user ? (
          <>
            <Typography sx={{ mr: 2 }}>{user.email}</Typography>
            <Button color="inherit" onClick={logout}>Logout</Button>
          </>
        ) : (
          <>
            <Button color="inherit" onClick={() => navigate("/login")}>Login</Button>
            <Button color="inherit" onClick={() => navigate("/register")}>Register</Button>
          </>
        )}
      </Toolbar>
    </AppBar>
  );
};

export default Header;
