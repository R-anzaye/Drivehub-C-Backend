import React from "react";
import { Toaster } from 'react-hot-toast';
import { BrowserRouter, Routes, Route } from "react-router-dom";
import Home from "./pages/Home";
import Notification from "./pages/Notification";
import NewCar from "./pages/NewCar";
import Report from "./pages/Report";
import Settings from "./pages/Settings";
import Profile from "./pages/Profile";
import CarDetails from "./components/CarDetails";
import { AuthProvider } from "./context/AuthContext"; // Import the AuthProvider

function App() {
  return (
    <>
    <Toaster position="top-right" reverseOrder={false} />
    <BrowserRouter>
      <AuthProvider> 
        <Routes>
          <Route path="/" element={<Home />} />
          <Route path="/cars/:carId" element={<CarDetails />}  />
          <Route path="/notification" element={<Notification />}  />
          <Route path="/newCar" element={<NewCar />} />
          <Route path="/reports" element={<Report />} />
          <Route path="/settings" element={<Settings />} />
          <Route path="/profile" element={<Profile />} />
        </Routes>
      </AuthProvider>
    </BrowserRouter>
    </>
  );
}

export default App;
