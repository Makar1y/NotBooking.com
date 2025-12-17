# Simple Template Engine

## Usage

After running `processTemplate` function, the output file will be generated and ready for use if no errors occurred.

Example:
```C
// Processes the template and generates the output file
// Returns 0 on success, non-zero on error.
int result = processTemplate("templates/template.html", "templates/output.html", "templates/data.json");
if (result != 0) {
    printf("Template processing failed.\n");
}
```

### Required files

for engine working you need to have:
- template file
- json file

with special syntax written below

## Template Syntax

### 1. Blocks

Format: `{? content ?}`  
Replace values inside block with data from json for each element in json data.

From our example [template](templates/template.html):
```html
{? 
<div class="hotel glass">
    <div class="hotel-image">
        <img src="{{image}}" alt="Hotel image" />
    </div>
    <div class="hotel-details glass">
        <div class="detail hotel-name">{{name}}</div>
        <div class="detail location">{{location}}</div>
        <div class="detail hotel-price">{{price}}€ / 24h</div>
        <div class="detail hotel-amenities">
            {%{amenities} <span>{{}}</span> %}
        </div>
        <div class="detail hotel-description">
            <p>
                {{description}}
            </p>
        </div>
        <a class="detail button" href="/hotels/?hotel={{name}}"><button type="button">Details</button></a>
    </div>
</div>
?}
```

### 2. Variables

Format: `{{key}}`  
Replaced by the value of `key` from the JSON object.

```html
<p>Hello, {{name}}!</p>
```

### 3. Arrays

Format: `{%{array_name} {{}} %}`  
this block copied for each element in array and paste array element in {{}}.

```html
{%{items} <span>{{}}</span> %}
```

### 4. JSON data

You can see in [data.json](templates/data.json) file example of JSON data structure.

## Test

For testing purposes, the module can be compiled and run as a standalone program:
```bash
gcc tmpEngine.c -D TEST -o tmpEngine_test
```
```bash
./tmpEngine_test templates/template.html templates/data.json templates/output.html
```

To compile the engine with your code:
```bash
gcc your_code.c tmpEngine.c -o your_program
```

## Files

- [**tmpEngine.c**](tmpEngine.c) - module source file.
- [**tmpEngine.h**](tmpEngine.h) - Header file.
- [**templates**](templates) - Directory with example files.
  - [**template.html**](templates/template.html): Example HTML template.
  - [**data.json**](templates/data.json): Example JSON data.
  - [**output.html**](templates/output.html): Example output.
