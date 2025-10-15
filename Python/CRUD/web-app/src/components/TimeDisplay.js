import React, { useEffect, useState } from "react";
import { Card, CardContent, Typography, CircularProgress } from "@mui/material";

const TimeDisplay = () => {
  const [time, setTime] = useState(null);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    const fetchTime = () => {
      fetch("http://127.0.0.1:5000/time") // Adjust Flask API URL if needed
        .then((response) => response.json())
        .then((data) => {
          setTime(new Date(data.time * 1000).toLocaleTimeString());
          setLoading(false);
        })
        .catch((error) => {
          console.error("Error fetching time:", error);
          setLoading(false);
        });
    };

    fetchTime();
    const interval = setInterval(fetchTime, 1000); // Update every second

    return () => clearInterval(interval); // Cleanup interval on unmount
  }, []);

  return (
    <Card style={{ maxWidth: 400, margin: "auto", marginTop: 20, textAlign: "center", padding: 10 }}>
      <CardContent>
        <Typography variant="h6">Current Time</Typography>
        {loading ? (
          <CircularProgress style={{ margin: "10px auto" }} />
        ) : (
          <Typography variant="h4" color="primary">{time}</Typography>
        )}
      </CardContent>
    </Card>
  );
};

export default TimeDisplay;
