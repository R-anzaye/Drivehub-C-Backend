import { toast } from 'react-hot-toast';
import React, { createContext, useState, useContext, useEffect } from 'react';
import { useNavigate } from 'react-router-dom';


const AuthContext = createContext();
export const useAuth = () => useContext(AuthContext);

export const AuthProvider = ({ children }) => {
  const [token, setToken] = useState(sessionStorage.getItem('token') || null);
  const [isAuthenticated, setIsAuthenticated] = useState(!!sessionStorage.getItem('token'));
  const [currentUser, setCurrentUser] = useState(JSON.parse(sessionStorage.getItem('user')) || null);
  const navigate = useNavigate();

  useEffect(() => {
    const storedToken = sessionStorage.getItem('token');
    const storedUser = sessionStorage.getItem('user');

    if (storedToken && storedUser) {
      setToken(storedToken);
      setCurrentUser(JSON.parse(storedUser));
      setIsAuthenticated(true);
      fetchProfile(storedToken);
    }
  }, []);

  const login = async (email, password) => {
    try {
      const response = await fetch('http://127.0.0.1:5555/login', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        credentials: 'include',
        body: JSON.stringify({ email, password }),
      });
      const data = await response.json();

      if (response.ok) {
        setToken(data.access_token);
        setIsAuthenticated(true);
        setCurrentUser(data.user);

        sessionStorage.setItem('token', data.access_token);
        sessionStorage.setItem('user', JSON.stringify(data.user));

        toast.success('Login successful!');
        navigate('/profile');
      } else {
        throw new Error(data.error);
      }
    } catch (error) {
      toast.error('Login failed! Please check your credentials.');
      console.error('Login failed:', error);
    }
  };

  const register = async (first_name, last_name, email, password, organisation) => {
    try {
      const response = await fetch('http://127.0.0.1:5555/register', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ first_name, last_name, email, password, organisation }),
      });
      const data = await response.json();

      if (response.ok) {
        toast.success('Registration successful! Logging you in...');
        login(email, password);
      } else {
        throw new Error(data.error);
      }
    } catch (error) {
      toast.error('Registration failed. Please try again.');
      console.error('Registration failed:', error);
    }
  };

  const logout = async () => {
    try {
      await fetch('http://127.0.0.1:5555/logout', { method: 'POST' });
      sessionStorage.removeItem('token');
      sessionStorage.removeItem('user');
      setToken(null);
      setIsAuthenticated(false);
      setCurrentUser(null);
      toast.success('Successfully logged out.');
      navigate('/');
    } catch (error) {
      toast.error('Logout failed. Please try again.');
      console.error('Logout failed:', error);
    }
  };
  const fetchProfile = async () => {
    try {
      const response = await fetch('http://127.0.0.1:5555/profile', {
        headers: { Authorization: `Bearer ${token}` },
      });
      const data = await response.json();
      if (response.ok) {
        setCurrentUser(data);
        sessionStorage.setItem('user', JSON.stringify(data));
      } else {
        throw new Error('Failed to fetch profile');
      }
    } catch (error) {
      console.error(error);
    }
  };
  const updateProfile = async (updatedUser) => {
    try {
      const response = await fetch(`http://127.0.0.1:5555/update_profile/${currentUser.id}`, {
        method: 'PUT',
        headers: {
          'Content-Type': 'application/json',
          Authorization: `Bearer ${token}`,
        },
        body: JSON.stringify(updatedUser),
      });

      const data = await response.json();
      if (response.ok) {
        setCurrentUser(data);
        sessionStorage.setItem('user', JSON.stringify(data));
        toast.success('Profile updated successfully!');
      } else {
        throw new Error('Failed to update profile');
      }
    } catch (error) {
      toast.error('Profile update failed. Please try again.');
      console.error(error);
    }
  };

  const deleteAccount = async () => {
    try {
      await fetch(`http://127.0.0.1:5555/delete_account/${currentUser.id}`, {
        method: 'DELETE',
        headers: { Authorization: `Bearer ${token}` },
      });
      toast.success('Account deleted successfully!');
      logout();
    } catch (error) {
      toast.error('Failed to delete account. Please try again.');
      console.error('Failed to delete account:', error);
    }
  };

  return (
    <AuthContext.Provider value={{ token, isAuthenticated, currentUser, login, register, logout, updateProfile, deleteAccount }}>
      {children}
    </AuthContext.Provider>
  );
};
