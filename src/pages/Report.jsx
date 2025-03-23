import React, { useState, useEffect } from "react";
import "./styles.css";
import Navbar from "../components/Navbar.jsx";
import { useAuth } from "../context/AuthContext";

function Report() {
  const [cars, setCars] = useState([]);
  const [error, setError] = useState(null);
  const { token, currentUser } = useAuth()
 
    useEffect(() => {
      const fetchCars = async () => {
        try {
          const response = await fetch(`http://127.0.0.1:5555/cars`, {  // Corrected the URL
            method: "GET",
            headers: {
              "Content-Type": "application/json",
              Authorization: `Bearer ${token}`,
            },
          });
    
          const data = await response.json();
          setProjects(data);
        } catch (err) {
          setError("Failed to fetch projects");
        }
      };
    
      fetchCars();
    }, [ token]);  // Added dependencies to ensure fetch runs correctly


  return (
    <>
      <Navbar />
      <div className="container">
        <main className="main-content">
          <h1 className="header-title">Reports</h1>
  
        </main>
      </div>
    </>
  )
}  

export default Report;