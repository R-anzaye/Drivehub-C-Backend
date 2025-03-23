import React, { useState } from "react";
import "./styles.css";
import Navbar from "../components/Navbar.jsx";
import Footer from "../components/Footer.jsx";
import { useAuth } from '../context/AuthContext';
import SuccessMessage from "../components/SuccessMessage";

function Home() {
  const [showForm, setShowForm] = useState(false);
  const [formType, setFormType] = useState('login');
  const [first_name, setFirstName] = useState('');
  const [last_name, setLastName] = useState('');
  const [password, setPassword] = useState('');
  const [confirmPassword, setConfirmPassword] = useState('');
  const [email, setEmail] = useState('');
  const [error, setError] = useState(null);
  const [organisation, setOrganisation] = useState('');
  const [showSuccessMessage, setShowSuccessMessage] = useState(false);
  const { login, register, currentUser } = useAuth();

  const handleLogin = async (e) => {
    e.preventDefault();
    try {
      await login(email, password);
      setShowSuccessMessage(true);
      setTimeout(() => {
        setShowSuccessMessage(false);
      }, 3000);
    } catch (err) {
      setError('Login failed.');
    }
  };

  const handleRegister = async (e) => {
    e.preventDefault();
    if (password !== confirmPassword) {
      setError('Passwords do not match');
      return;
    }
    try {
      await register(first_name, last_name, email, password, organisation);
      setShowSuccessMessage(true);
      setTimeout(() => {
        setShowSuccessMessage(false);
      }, 3000);
    } catch (err) {
      setError('Registration failed.');
    }
  };

  const handleGetStartedClick = () => {
    setShowForm(true);
    setFormType("login");
  };

  const handleRegisterClick = () => {
    setFormType("register");
    setShowForm(true);
  };

  return (
    <>
      <Navbar />
      <div className="container">
        <main className="main-content">
          <div className="header-title">
            <h1 className="header-linkup-title">DriveHub</h1>
          </div>
          <div className="header">
          <img src="/src/teall.webp" alt="Image" className="header-image" />
            {showForm ? (
              <div className="form-container">
                {error && <p className="error-message">{error}</p>}
                {formType === "login" ? (
                  <form className="login-form" onSubmit={handleLogin}>
                    <h2>Login</h2>
                    <input
                      type="email"
                      placeholder="Email"
                      required
                      value={email}
                      onChange={(e) => setEmail(e.target.value)}
                    />
                    <input
                      type="password"
                      placeholder="Password"
                      required
                      value={password}
                      onChange={(e) => setPassword(e.target.value)}
                    />
                    <button type="submit" className="register-btn">Login</button>
                    <p>Don't have an account?</p>
                    <button type="button" className="register-btn" onClick={handleRegisterClick}>Register</button>
                  </form>
                ) : (
                  <form className="register-form" onSubmit={handleRegister}>
                    <h2>Register</h2>
                    <input
                      type="text"
                      placeholder="First Name"
                      required
                      value={first_name}
                      onChange={(e) => setFirstName(e.target.value)}
                    />
                    <input
                      type="text"
                      placeholder="Last Name"
                      required
                      value={last_name}
                      onChange={(e) => setLastName(e.target.value)}
                    />
                    <input
                      type="email"
                      placeholder="Email"
                      required
                      value={email}
                      onChange={(e) => setEmail(e.target.value)}
                    />
                    <input
                      type="text"
                      placeholder="Organisation"
                      required
                      value={organisation}
                      onChange={(e) => setOrganisation(e.target.value)}
                    />
                    <input
                      type="password"
                      placeholder="Password"
                      required
                      value={password}
                      onChange={(e) => setPassword(e.target.value)}
                    />
                    <input
                      type="password"
                      placeholder="Confirm Password"
                      required
                      value={confirmPassword}
                      onChange={(e) => setConfirmPassword(e.target.value)}
                    />
                    <button type="submit" className="register-btn">Register</button>
                    <p>Already have an account?</p>
                    <button type="button" className="register-btn" onClick={handleGetStartedClick}>Login</button>
                  </form>
                )}
              </div>
            ) : (
              <div className="header-content">
                <h1>Welcome to <span>DriveHub</span></h1>
                <p><span>Efficiently manage</span> your dealership’s inventory, sales, and vehicle records with ease.</p>
                <p><span>Streamline your operations</span> with real-time car tracking, pricing updates, and seamless customer interactions.</p>

                {!currentUser && (
                  <button className="get-started-btn" onClick={handleGetStartedClick}>Get Started</button>
                )}
              </div>
            )}
          </div>

          <div className="choose">
            <h1>Why choose DriveHub?</h1>
            <img src="src/thinking.webp" alt="Thinking" />
          </div>
          <div className="cards">
            <div className="card">
            <img src="/src/track.png" alt="Simplified referral tracking" />
              <h1>Inventory Management</h1>
              <p>Easily add, edit, and remove vehicles while keeping track of available stock in real time.</p>
            </div>
            <div className="card">
            <img src="/src/tuck.png" alt="Transparent Commission Management" />
              <h1>Sales and Customer Records</h1>
              <p>Maintain detailed records of sales, clients, and transaction history for better customer engagement.</p>
            </div>
            <div className="card">
            <img src="/src/buss.png" alt="Boost Your Business" />
              <h1>Advanced Search & Filtering</h1>
              <p>Quickly find vehicles based on make, model, price, and other parameters to speed up operations.</p>
            </div>
          </div>

          <Footer />
        </main>
      </div>

      {showSuccessMessage && <SuccessMessage message="Action successful!" />}
    </>
  );
}

export default Home;
