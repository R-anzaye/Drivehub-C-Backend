import React, { useState, useEffect } from "react";
import { Link } from "react-router-dom";
import AddCar from "../components/AddCar.jsx";
import Navbar from "../components/Navbar.jsx";
import { useAuth } from "../context/AuthContext.jsx"; 

const NewCar = () => {
    const [cars, setCars] = useState([]);
    const [showCarPopup, setShowCarPopup] = useState(false);
    const [error, setError] = useState(null);

    const { token, currentUser } = useAuth(); 

    useEffect(() => {
        const fetchCars = async () => {
            try {
                const response = await fetch(`http://127.0.0.1:5555/cars`, {
                    method: 'GET',
                    headers: { 
                        "Content-Type": "application/json",
                        Authorization: `Bearer ${token}` }
                });

                if (!response.ok) {
                    throw new Error('Failed to fetch cars');
                }

                const data = await response.json();
                setCars(data);
            } catch (error) {
                setError(error.message);
            }
        };

        if (token) {
            fetchCars();
        }
    }, [token]);

    const handleAddCar = (newCar) => {
        setCars([...cars, newCar]);
        setShowCarPopup(false);
    };

    const handleDeleteCar = async (carId) => {
        try {
            const response = await fetch(`http://127.0.0.1:5555/cars/${carId}`, {
                method: 'DELETE',
                headers: {
                    "Content-Type": "application/json",
                    Authorization: `Bearer ${token}`
                }
            });

            if (!response.ok) {
                throw new Error('Failed to delete car');
            }

            // Remove the deleted project from the state
            setCars(cars.filter(car => car.id !== carId));
        } catch (error) {
            setError(error.message);
        }
    };

    return (
        <>
            <Navbar />
            <div className="container">
                <main className="main-content">
                    <div className="header-title">
                        <h1 className="header-linkup-title">Inventory</h1>
                        <button className="get-started-btn" onClick={() => setShowCarPopup(true)}>
                                Add Car
                            </button>
                    </div>
                    <div className="header">
                        <div className="header-cont">
                         
                        </div>
                    </div>

                    <div className="project-list">
                        {cars.map((car) => (
                            <div key={car.id} className="referral-item">
                                <p><strong>Car Name:</strong> {car.car_name}</p>
                                <p><strong>year of manufacture:</strong> {car.year_of_manufacture}</p>
                                <p><strong>Car Value:</strong> {car.car_value}</p>
                                <p><strong>Photo:</strong> {car.photo}</p>
                                <div className="flex justify-between items-center">
    <Link to={`/cars/${car.id}`}>
    <button className="text-[#197278] underline hover:text-[#ecd6cb] transition-colors duration-300">
    View More Details
</button>

    </Link>
    <button
        className="delete-btn hover:bg-red-700 transition duration-300 ease-in-out ml-1 w-auto px-5 py-1 text-md bg-red-500 text-white rounded"
        onClick={() => handleDeleteProject(project.id)}
    >
        Delete
    </button>
</div>

                            </div>
                        ))}
                    </div>

                    {showCarPopup && (
                        <AddCar
                            onClose={() => setShowCarPopup(false)}
                            onSave={handleAddCar}
                        />
                    )}

                    {error && <p className="error-message">{error}</p>}
                </main>
            </div>
        </>
    );
};

export default NewCar;

