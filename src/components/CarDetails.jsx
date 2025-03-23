import React, { useState, useEffect } from "react";
import { useParams } from "react-router-dom";

import { useAuth } from "../context/AuthContext"; 
import Navbar from "./Navbar";

const CarDetails = () => {
    const { carId } = useParams(); 
    const [car, setcar] = useState(null);
    const [error, setError] = useState(null);
    const { token, currentUser } = useAuth(); 

    useEffect(() => {
        const fetchcarDetails = async () => {
            try {
                const response = await fetch(`http://127.0.0.1:5555/cars/${carId}`, {
                    method: 'GET',
                    headers: { Authorization: `Bearer ${token}` },
                });

                if (!response.ok) {
                    throw new Error('Failed to fetch car details');
                }

                const data = await response.json();
                setcar(data);
                setEditForm({
                    car_name: data.car_name,
                    year_of_manufacture: data.year_of_manufacture,
                    car_value: data.car_value,
                    photo: data.photo
                }); 
            } catch (error) {
                setError(error.message);
            }
        };

       

        if (token && carId) {
            fetchcarDetails(); 
            
        }
    }, [carId, token]);



    if (!car) return <p>Loading...</p>;

    return (
        <>
        <Navbar />
        <div className="container">
            <main className="main-content">
                <h1 className="header-title">{car.car_name}</h1>

                <div className="car-detail">
                    <h1>car Details</h1>
                    {car && (
                        <div className="car-info">
                            <p><strong>car Name:</strong> {car.car_name}</p>
                            <p><strong>Year of Manufacture:</strong> {car.year_of_manufacture}</p>
                            <p><strong>Car Value:</strong> {car.car_value}</p>
                            <p><strong>Photo:</strong> {car.photo}</p>
                         

                            {/* Show the Add Referral button only if current user is the car creator */}
                            {currentUser.id === car.user_id && (
                                <button
                                    className="add-referral-btn"
                                    onClick={() => setShowReferralPopup(true)}
                                >
                                    Add Referral
                                </button>
                            )}
                        </div>
                    )}

                    {showReferralPopup && (
                        <SendReferral
                            carId={carId}
                            onClose={() => setShowReferralPopup(false)}
                            onSave={handleSaveReferral}
                        />
                    )}

                    <h1>Referrals</h1>
                    <div className="referral-list">
                        {referrals.length > 0 ? (
                            referrals.map((referral) => (
                                <div key={referral.id} className="referral-item">
                                    <p><strong>Email:</strong> {referral.recipient_email}</p>
                                    <p><strong>Date:</strong> {referral.referral_date}</p>
                                    <p><strong>Status:</strong> {referral.status}</p>
                                    <p><strong>Commission Rate:</strong> {referral.commission_rate}</p>
                                    <p><strong>Commission Amount:</strong> {referral.commission_amount}</p>
                                    <p><strong>Commission Type:</strong> {referral.commission_type}</p>

                                    {/* Conditionally show Accept/Decline buttons if current user is the referred person */}
                                    {currentUser.id === referral.recipient_id && referral.status === 'pending' && (
                                        <div className="flex space-x-4 mt-2">
                                            <button
                                                onClick={() => handleReferralAction(referral.id, 'accept')}
                                                className="bg-green-500 text-white px-4 py-2 rounded hover:bg-green-600"
                                            >
                                                Accept Referral
                                            </button>
                                            <button
                                                onClick={() => handleReferralAction(referral.id, 'decline')}
                                                className="bg-red-500 text-white px-4 py-2 rounded hover:bg-red-600"
                                            >
                                                Decline Referral
                                            </button>
                                        </div>
                                    )}

                                    {/* Display commission status inside each referral */}
                                    <div className="commission-section mt-4">
            <h2>Commission Details</h2>
            {referral.commission ? (
                <>
                    <p><strong>Amount:</strong> {referral.commission.amount}</p>
                    <p><strong>Status:</strong> {referral.commission.status}</p>
                    <p><strong>Due Date:</strong> {referral.commission.due_date}</p>

                    {/* ... commission status update buttons ... */}
                </>
            ) : (
                <p>No commission information available</p>
            )}

                                        {/* Show status update buttons for referred user */}
                                        {currentUser.id === referral.recipient_id && (
                                            <div className="flex space-x-4 mt-2">
                                                <button
                                                    onClick={() => handleCommissionStatusUpdate(referral.id, 'paid')}
                                                    className="bg-blue-500 text-white px-4 py-2 rounded hover:bg-blue-600"
                                                >
                                                    Mark as Paid
                                                </button>
                                                <button
                                                    onClick={() => handleCommissionStatusUpdate(referral.id, 'overdue')}
                                                    className="bg-yellow-500 text-white px-4 py-2 rounded hover:bg-yellow-600"
                                                >
                                                    Mark as Overdue
                                                </button>
                                            </div>
                                        )}
                                    </div>

                                </div>
                            ))
                        ) : (
                            <p>No referrals found for this car.</p>
                        )}
                    </div>
                </div>
            </main>
        </div>
        </>
    );
};

export default CarDetails;
