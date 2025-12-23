import hotels from "/hotels.js";

function createHotelCard(hotel) {
    const hotelCard = document.createElement("div");
    hotelCard.className = "hotel glass";

    const hotelImage = document.createElement("div");
    hotelImage.className = "hotel-image";

    const img = document.createElement("img");
    img.src = hotel.image;
    img.alt = "Hotel image";
    hotelImage.appendChild(img);
    hotelCard.appendChild(hotelImage);

    const hotelDetails = document.createElement("div");
    hotelDetails.className = "hotel-details glass";

    const nameDiv = document.createElement("div");
    nameDiv.className = "detail hotel-name";
    nameDiv.textContent = hotel.name;
    hotelDetails.appendChild(nameDiv);

    const locationDiv = document.createElement("div");
    locationDiv.className = "detail location";
    locationDiv.textContent = hotel.location;
    hotelDetails.appendChild(locationDiv);

    const descriptionDiv = document.createElement("div");
    descriptionDiv.className = "detail hotel-description";

    const p = document.createElement("p");
    p.textContent = hotel.description;
    descriptionDiv.appendChild(p);
    hotelDetails.appendChild(descriptionDiv);

    const amenitiesContainer = document.createElement("div");
    amenitiesContainer.className = "detail hotel-amenities-container";

    const amenitiesHeader = document.createElement("h3");
    amenitiesHeader.className = "hotel-amenities-header";
    amenitiesHeader.textContent = "Amenities:";
    amenitiesContainer.appendChild(amenitiesHeader);

    const amenitiesList = document.createElement("ul");
    amenitiesList.className = "hotel-amenities";
    
    hotel.amenities.forEach((amenity) => {
        const li = document.createElement("li");
        li.textContent = amenity;
        amenitiesList.appendChild(li);
    });
    amenitiesContainer.appendChild(amenitiesList);
    hotelDetails.appendChild(amenitiesContainer);

    hotelCard.appendChild(hotelDetails);

    return hotelCard;
}

document.addEventListener("DOMContentLoaded", () => {
    const hotelSection = document.getElementById("hotel");
    const priceSpan = document.getElementById("price");
    const params = new URLSearchParams(document.location.search);
    const hotelName = params.get("hotel");

    const hotel = hotels.find(hotel => hotel.name === hotelName);
    if (hotelName != null) {
        if (hotel) {
            hotelSection.appendChild(createHotelCard(hotel));
            priceSpan.append(`${hotel.price.toLocaleString("lt-LT")}€`);
        } else {
            window.location.replace("/hotels/" + hotelName);
        }
    } else {
        window.location.replace("/");
    }
    
});