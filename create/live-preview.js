document.addEventListener('DOMContentLoaded', function() {
  // Get form inputs
  const hotelNameInput = document.getElementById('hotel-name');
  const hotelLocationSelect = document.getElementById('hotel-location');
  const hotelDescriptionInput = document.getElementById('hotel-description');
  const hotelPriceInput = document.getElementById('hotel-price');
  const hotelAmenitiesInput = document.getElementById('hotel-amenities');
  const hotelImageInput = document.getElementById('hotel-image');
  
  // Get preview elements
  const previewName = document.querySelector('.preview-details .hotel-name');
  const previewLocation = document.querySelector('.preview-details .location span');
  const previewDescription = document.querySelector('.preview-details .hotel-description');
  const previewPrice = document.querySelector('.preview-details .hotel-price');
  const previewAmenities = document.querySelector('.preview-details .hotel-amenities');
  const previewImage = document.querySelector('.image-preview img');
  
  // Update preview when inputs change
  hotelNameInput.addEventListener('input', updateNamePreview);
  hotelLocationSelect.addEventListener('change', updateLocationPreview);
  hotelDescriptionInput.addEventListener('input', updateDescriptionPreview);
  hotelPriceInput.addEventListener('input', updatePricePreview);
  hotelAmenitiesInput.addEventListener('input', updateAmenitiesPreview);
  hotelImageInput.addEventListener('change', updateImagePreview);
  
  function updateNamePreview() {
    const name = hotelNameInput.value.trim() || 'Hotel name';
    previewName.textContent = name;
  }
  
  function updateLocationPreview() {
    const location = hotelLocationSelect.value || 'Planet';
    previewLocation.textContent = location;
  }
  
  function updateDescriptionPreview() {
    const description = hotelDescriptionInput.value.trim() || 'Hotel description...';
    previewDescription.textContent = description;
  }
  
  function updatePricePreview() {
    const price = hotelPriceInput.value ? parseFloat(hotelPriceInput.value).toFixed(2) : '00.00';
    previewPrice.textContent = `${price}€ / 24h`;
  }
  
  function updateAmenitiesPreview() {
    const amenitiesText = hotelAmenitiesInput.value.trim();
    
    if (!amenitiesText) {
      previewAmenities.innerHTML = '';
      return;
    }
    
    // Split amenities by comma and create span elements
    const amenitiesArray = amenitiesText.split(',').map(amenity => amenity.trim()).filter(amenity => amenity);
    
    previewAmenities.innerHTML = amenitiesArray
      .map(amenity => `<span>${amenity}</span>`)
      .join('');
  }
  
  function updateImagePreview(event) {
    const file = event.target.files[0];
    
    if (file) {
      const reader = new FileReader();
      
      reader.onload = function(e) {
        previewImage.src = e.target.result;
      };
      
      reader.readAsDataURL(file);
    }
  }
  
  // Initialize preview with default values
  updateNamePreview();
  updateLocationPreview();
  updateDescriptionPreview();
  updatePricePreview();
  updateAmenitiesPreview();
});