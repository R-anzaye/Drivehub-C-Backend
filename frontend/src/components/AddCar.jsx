import React, { useState } from "react";
import { useAuth } from "../context/AuthContext"; 

const AddCar = ({ onSave, onClose }) => {
    const [car_name, setCar_name] = useState('');
    const [year_of_manufacture, setYear_of_manufacture] = useState('');
    const [car_value, setCar_value] = useState('');
    const [photo, setPhoto] = useState(null);
    const [preview, setPreview] = useState(null);
    const { token } = useAuth(); 
    const [error, setError] = useState(null);

    
    const handleFileChange = (e) => {
        const file = e.target.files[0];
        setPhoto(file);

        
        if (file) {
            const reader = new FileReader();
            reader.onloadend = () => {
                setPreview(reader.result);
            };
            reader.readAsDataURL(file);
        }
    };

   
    const handleAddCar = async (e) => {
        e.preventDefault();

        const formData = new FormData();
        formData.append("car_name", car_name);
        formData.append("year_of_manufacture", year_of_manufacture);
        formData.append("car_value", car_value);
        formData.append("photo", photo); 
        formData.append("client_org", client_org);

        try {
            const response = await fetch('http://127.0.0.1:5555/cars', {
                method: 'POST',
                headers: {
                    'Authorization': `Bearer ${token}` 
                },
                body: formData 
            });

            if (!response.ok) {
                const errorData = await response.json();
                throw new Error(errorData.error || 'Failed to add car');
            }

            const data = await response.json();
            onSave(data); 
            onClose(); 
        } catch (error) {
            setError(error.message); 
        }

        setCar_name('');
        setYear_of_manufacture('');
        setCar_value('');
        setPhoto(null);
        setPreview(null);
        setClient_org('');
    };

    return (
        <div className="modal">
            <div className="modal-content">
                <span className="close-btn" onClick={onClose}>&times;</span>
                <h2>Add Car</h2>
                <form onSubmit={handleAddCar}>
                    <input
                        type="text"
                        value={car_name}
                        onChange={(e) => setCar_name(e.target.value)}
                        placeholder="Enter Car name"
                        required
                    />
                    <input
                        type="text"
                        value={year_of_manufacture}
                        onChange={(e) => setYear_of_manufacture(e.target.value)}
                        placeholder="Enter Year of manufacture"
                        required
                    />
                    <input
                        type="text"
                        value={car_value}
                        onChange={(e) => setCar_value(e.target.value)}
                        placeholder="Enter Car value"
                        required
                    />
                    
                 
                    <input 
                        type="file" 
                        accept="image/*" 
                        onChange={handleFileChange} 
                        required 
                    />

                   
                    {preview && (
                        <div>
                            <p>Image Preview:</p>
                            <img src={preview} alt="Preview" style={{ width: "200px", marginTop: "10px" }} />
                        </div>
                    )}

                
                    <button type="submit">Add car</button>
                </form>
                {error && <p className="error">{error}</p>}
            </div>
        </div>
    );
};

export default AddCar;
