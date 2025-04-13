import React, { useState } from 'react';
import './profile.css';
import Navbar from '../components/Navbar';
import { useAuth } from '../context/AuthContext';
import { useNavigate } from 'react-router-dom';

function Profile() {
  const { currentUser, logout, updateProfile } = useAuth();
  const navigate = useNavigate();
  const [isEditing, setIsEditing] = useState(false);
  const [firstName, setFirstName] = useState(currentUser?.first_name || '');
  const [lastName, setLastName] = useState(currentUser?.last_name || '');
  const [organisation, setOrganisation] = useState(currentUser?.organisation || '');

  const handleLogout = () => {
    logout();
    navigate('/', { replace: true });
  };

  const handleUpdateProfile = async () => {
    try {
      if (!firstName.trim() || !lastName.trim()) {
        throw new Error("First name and last name cannot be empty");
      }

      await updateProfile({
        first_name: firstName,
        last_name: lastName,
        organisation: organisation,
      });
      setIsEditing(false);
      window.location.reload()
    } catch (error) {
      console.error('Error updating profile:', error);
      alert(error.message); 
    }
  };

  const handleEditClick = () => {
    setIsEditing(true);
  };

  if (!currentUser) {
    return (
        <>
            <Navbar />
            <h1 className="hhh">Profile</h1>
            <div className="profi">
                <p className="error-message">⚠️ Oops! User data not found.</p>
            </div>
        </>
    );
}


  return (
    <>
      <Navbar />
      <div className="profile-page">
        <div className="content">
          <div className="content__cover">
            <div className="content__avatar"></div>
          </div>
          <div className="content__update-button">
            <button onClick={handleEditClick}>
              Edit Profile
            </button>
          </div>
          <div className="content__title">
            <h1>
              {isEditing ? (
                <>
                  <input
                    type="text"
                    value={firstName}
                    onChange={(e) => setFirstName(e.target.value)}
                    className={`required-field ${firstName.trim() ? '' : 'input-error'}`}
                  />{' '}
                  <input
                    type="text"
                    value={lastName}
                    onChange={(e) => setLastName(e.target.value)}
                    className={`required-field ${lastName.trim() ? '' : 'input-error'}`}
                  />
                </>
              ) : (
                `${currentUser.first_name} ${currentUser.last_name}`
              )}
            </h1>
          </div>
          <div className="content__description">
            <p>
              {isEditing ? (
                <input
                  type="text"
                  value={organisation}
                  onChange={(e) => setOrganisation(e.target.value)}
                />
              ) : (
                currentUser.organisation
              )}
            </p>
            <h3>Email</h3>
            <p>{currentUser.email}</p>
          </div>
          {isEditing && (
            <div className="modal">
              <div className="modal-content">
                <h2>Edit Profile</h2>
                <input
                  type="text"
                  value={firstName}
                  onChange={(e) => setFirstName(e.target.value)}
                  placeholder="First Name"
                  className={`required-field ${firstName.trim() ? '' : 'input-error'}`}
                />
                <input
                  type="text"
                  value={lastName}
                  onChange={(e) => setLastName(e.target.value)}
                  placeholder="Last Name"
                  className={`required-field ${lastName.trim() ? '' : 'input-error'}`}
                />
                <input
                  type="text"
                  value={organisation}
                  onChange={(e) => setOrganisation(e.target.value)}
                  placeholder="Organisation"
                />
                <button onClick={handleUpdateProfile} disabled={!firstName.trim() || !lastName.trim()}>
                  Save Changes
                </button>
              </div>
            </div>
          )}
          {!isEditing && (
            <div className="content__button">
              <button className="button" onClick={handleLogout}>
                Logout
              </button>
            </div>
          )}
        </div>
      </div>
    </>
  );
}

export default Profile;
