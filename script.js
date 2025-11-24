import hotels from "./hotels.js";

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

    const priceDiv = document.createElement("div");
    priceDiv.className = "detail hotel-price";
    priceDiv.textContent = `${hotel.price.toLocaleString("lt-LT")}€ / 24h`;
    hotelDetails.appendChild(priceDiv);

    const amenitiesDiv = document.createElement("div");
    amenitiesDiv.className = "detail hotel-amenities";
    hotel.amenities.forEach((amenity) => {
        const span = document.createElement("span");
        span.textContent = amenity;
        amenitiesDiv.appendChild(span);
    });
    hotelDetails.appendChild(amenitiesDiv);

    const descriptionDiv = document.createElement("div");
    descriptionDiv.className = "detail hotel-description";

    const p = document.createElement("p");
    p.textContent = hotel.description;
    descriptionDiv.appendChild(p);
    hotelDetails.appendChild(descriptionDiv);

    const buttonElement = document.createElement("a");
    buttonElement.className = "detail button";
    buttonElement.href = "/hotels/?hotel=" + encodeURIComponent(hotel.name);

    const button = document.createElement("button");
    button.textContent = "Details";
    button.type = "button"
    buttonElement.appendChild(button);
    hotelDetails.appendChild(buttonElement);
    hotelCard.appendChild(hotelDetails);

    return hotelCard;
}

document.getElementById("search-button").addEventListener("click", () => {
    const planetSelect = document.getElementById("planet-select");
    const searchText = document.getElementById("search-text");

    const selectedPlanet = planetSelect.value;
    const searchQuery = searchText.value.toLowerCase();

    const hotelsGrid = document.querySelector(".hotels-grid");
    hotelsGrid.innerHTML = "";

    const filteredHotels = hotels.filter(hotel => {
        const matchesPlanet = selectedPlanet === "" || hotel.location === selectedPlanet;
        const matchesSearch = hotel.name.toLowerCase().includes(searchQuery);
        return matchesPlanet && matchesSearch;
    });

    filteredHotels.forEach(hotel => {
        const hotelCard = createHotelCard(hotel);
        hotelsGrid.appendChild(hotelCard);
    });
});

document.addEventListener("DOMContentLoaded", () => {
    const hotelsGrid = document.querySelector(".hotels-grid");
    hotels.forEach(hotel => {
        const hotelCard = createHotelCard(hotel);
        hotelsGrid.appendChild(hotelCard);
    });
});