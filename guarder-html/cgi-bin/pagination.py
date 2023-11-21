import os
import re

if os.environ.get("REQUEST_METHOD") != "GET":
  exit()

# List of divs
divs = ['''<div class = "container" style="background-color: #db9595"><h1>#db9595</h1></div>''',
         '''<div class = "container" style="background-color: #dba395"><h1>#dba395</h1></div>''', 
            '''<div class = "container" style="background-color: #dbb295"><h1>#dbb295</h1></div>''', 
             '''<div class = "container" style="background-color: #dbbe95"><h1>#dbbe95</h1></div>''', 
          '''<div class = "container" style="background-color: #dbd195"><h1>#dbd195</h1></div>''',
          '''<div class = "container" style="background-color: #c8db95"><h1>#c8db95</h1></div>''',
          '''<div class = "container" style="background-color: #a3db95"><h1>#a3db95</h1></div>''',
          '''<div class = "container" style="background-color: #95dbb2"><h1>#95dbb2</h1></div>''',
          '''<div class = "container" style="background-color: #95dbd4"><h1>#95dbd4</h1></div>''',
          '''<div class = "container" style="background-color: #95cbdb"><h1>#95cbdb</h1></div>''',
          '''<div class = "container" style="background-color: #95bcdb"><h1>#95bcdb</h1></div>''',
          '''<div class = "container" style="background-color: #959edb"><h1>#959edb</h1></div>''',
          '''<div class = "container" style="background-color: #a695db"><h1>#a695db</h1></div>''',
          '''<div class = "container" style="background-color: #bc95db"><h1>#bc95db</h1></div>''',
          '''<div class = "container" style="background-color: #d395db"><h1>#d395db</h1></div>''']

# Generate the HTML response
print('''
<!DOCTYPE html>
<html>

<head>
  <!-- Basic -->
  <meta charset="utf-8" />
  <meta http-equiv="X-UA-Compatible" content="IE=edge" />
  <!-- Mobile Metas -->
  <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no" />
  <!-- Site Metas -->
  <meta name="keywords" content="" />
  <meta name="description" content="" />
  <meta name="author" content="" />
  <link rel="shortcut icon" href="../images/favicon.png" type="image/x-icon">

    <title>Webserv</title>

  <!-- bootstrap core css -->
  <link rel="stylesheet" type="text/css" href="css/bootstrap.css" />

  <!-- fonts style -->
  <link href="https://fonts.googleapis.com/css?family=Open+Sans:400,700|Poppins:400,600,700&display=swap" rel="stylesheet" />

  <!-- Custom styles for this template -->
  <link href="css/style.css" rel="stylesheet" />
  <!-- responsive style -->
  <link href="css/responsive.css" rel="stylesheet" />
</head>

<body class="sub_page">
  <div class="hero_area">
    <!-- header section strats -->
    <div class="hero_bg_box">
      <div class="img-box">
        <img src="images/hero-bg.jpg" alt="">
      </div>
    </div>

    <header class="header_section">
      <div class="header_top">
        <div class="container-fluid">
          <div class="contact_link-container">
            <a href="" class="contact_link1">
              <i class="fa fa-map-marker" aria-hidden="true"></i>
              <span>
                Lorem ipsum dolor sit amet,
              </span>
            </a>
            <a href="" class="contact_link2">
              <i class="fa fa-phone" aria-hidden="true"></i>
              <span>
                Call : +01 1234567890
              </span>
            </a>
            <a href="" class="contact_link3">
              <i class="fa fa-envelope" aria-hidden="true"></i>
              <span>
                demo@gmail.com
              </span>
            </a>
          </div>
        </div>
      </div>
      <div class="header_bottom">
        <div class="container-fluid">
          <nav class="navbar navbar-expand-lg custom_nav-container">
            <a class="navbar-brand" href="index.html">
              <span>
                Guarder
              </span>
            </a>
            <button class="navbar-toggler" type="button" data-toggle="collapse" data-target="#navbarSupportedContent" aria-controls="navbarSupportedContent" aria-expanded="false" aria-label="Toggle navigation">
              <span class=""></span>
            </button>

            <div class="collapse navbar-collapse ml-auto" id="navbarSupportedContent">
              <ul class="navbar-nav  ">
                <li class="nav-item ">
                  <a class="nav-link" href="index.html">Home <span class="sr-only">(current)</span></a>
                </li>
                <li class="nav-item active">
                  <a class="nav-link" href="about.html"> About</a>
                </li>
                <li class="nav-item">
                  <a class="nav-link" href="service.html"> Services </a>
                </li>
                <li class="nav-item">
                  <a class="nav-link" href="guard.html"> Guards </a>
                </li>
                <li class="nav-item">
                  <a class="nav-link" href="contact.html">Contact us</a>
                </li>
              </ul>
            </div>
          </nav>
        </div>
      </div>
    </header>
    <!-- end header section -->
  </div>

  <!-- about section -->

  <section class="about_section layout_padding">
    <div class="container">
      <div class="row">
        <div class="col-md-6 px-0">
          <div class="img_container">
            <div class="img-box">
              <img src="images/about-img.jpg" alt="" />
            </div>
          </div>
        </div>
        <div class="col-md-6 px-0">
          <div class="detail-box">
            <div class="heading_container ">
              <h2>
                Pagination Example
              </h2>
            </div>
            <p>
              Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do
              eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut
              enim ad minim veniam, quis nostrud exercitation ullamco laboris
              nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor
              in reprehenderit in voluptate velit
            </p>
            <!-- <div class="btn-box">
				<form action="/cgi-bin/printrequest.py" method="POST" enctype="multipart/form-data">
					<a href="">Read More</a>
				</form>
            </div> -->
			<form action="cgi-bin/printrequest.py" method="POST" enctype="text/plain">
				<div class="mb-3">
					<input type="text" id="text-input" name="user_input" maxlength="200">
				</div>
				<div class="contact_form-container btn-box">
					<button type="submit" class="btn btn-primary">Generate POST request</button>
				</div>
			</form>
          </div>
        </div>
      </div>
    </div>
    <br>''')

query_string = os.environ.get("QUERY_STRING")
key, page_number = query_string.split('=')
if not re.match(r"^\d+$", page_number):
  print("<div class='container'><p>Invalid page number. Please provide a number between 1 and 3</p></div>")
else:
  page_number = int(page_number)
  if 0 < page_number < 4:
  # Calculate the start and end index for the divs to display
    items_per_page = 5  # Adjust this to change the number of divs per page
    start_index = (page_number - 1) * items_per_page
    end_index = start_index + items_per_page
# Display the selected divs
    for i in range(start_index, min(end_index, len(divs))):
      print(divs[i])
  else:
    print("<div class='container'><p>Invalid page number. Please provide a number between 1 and 3</p></div>")

print('''
   <nav class="container" aria-label="...">
      <ul class="justify-content-center pagination pagination-lg">
        <li class="page-item"><a class="page-link" href="../about.html?page=1">1</a></li>
        <li class="page-item"><a class="page-link" href="../about.html?page=2">2</a></li>
        <li class="page-item"><a class="page-link" href="../about.html?page=3">3</a></li>
      </ul>
    </nav>
  </section>

  <script>
    function loadPage() {
        // Get the query string from the current URL
        var queryString = window.location.search;
        
        // Extract the 'page' query parameter
        var page = new URLSearchParams(queryString).get('page');
        
        // If 'page' is not null, load the corresponding Python script
        if (page) {
            var script = document.createElement('script');
            script.src = 'pagination.py' + queryString;
            document.body.appendChild(script);
        }
    }

    // Call the loadPage function when the page loads
    window.onload = loadPage;
</script>
  
  <!-- end about section -->

  <!-- info section -->
  <section class="info_section ">
    <div class="container">
      <div class="row">
        <div class="col-md-3">
          <div class="info_logo">
            <a class="navbar-brand" href="../index.html">
              <span>
                Guarder
              </span>
            </a>
            <p>
              dolor sit amet, consectetur magna aliqua. Ut enim ad minim veniam, quisdotempor incididunt r
            </p>
          </div>
        </div>
        <div class="col-md-3">
          <div class="info_links">
            <h5>
              Useful Link
            </h5>
            <ul>
              <li>
                <a href="">
                  dolor sit amet, consectetur
                </a>
              </li>
              <li>
                <a href="">
                  magna aliqua. Ut enim ad
                </a>
              </li>
              <li>
                <a href="">
                  minim veniam,
                </a>
              </li>
              <li>
                <a href="">
                  quisdotempor incididunt r
                </a>
              </li>
            </ul>
          </div>
        </div>
        <div class="col-md-3">
          <div class="info_info">
            <h5>
              Contact Us
            </h5>
          </div>
          <div class="info_contact">
            <a href="" class="">
              <i class="fa fa-map-marker" aria-hidden="true"></i>
              <span>
                Lorem ipsum dolor sit amet,
              </span>
            </a>
            <a href="" class="">
              <i class="fa fa-phone" aria-hidden="true"></i>
              <span>
                Call : +01 1234567890
              </span>
            </a>
            <a href="" class="">
              <i class="fa fa-envelope" aria-hidden="true"></i>
              <span>
                demo@gmail.com
              </span>
            </a>
          </div>
        </div>
        <div class="col-md-3">
          <div class="info_form ">
            <h5>
              Newsletter
            </h5>
            <form action="#">
              <input type="email" placeholder="Enter your email">
              <button>
                Subscribe
              </button>
            </form>
            <div class="social_box">
              <a href="">
                <i class="fa fa-facebook" aria-hidden="true"></i>
              </a>
              <a href="">
                <i class="fa fa-twitter" aria-hidden="true"></i>
              </a>
              <a href="">
                <i class="fa fa-youtube" aria-hidden="true"></i>
              </a>
              <a href="">
                <i class="fa fa-instagram" aria-hidden="true"></i>
              </a>
            </div>
          </div>
        </div>
      </div>
    </div>
  </section>

  <!-- end info_section -->




  <!-- footer section -->
  <footer class="container-fluid footer_section">
    <p>
      &copy; <span id="currentYear"></span> All Rights Reserved. Design by
      <a href="https://html.design/">Free Html Templates</a>
    </p>
  </footer>
  <!-- footer section -->

  <script src="js/jquery-3.4.1.min.js"></script>
  <script src="js/bootstrap.js"></script>
  <script src="js/custom.js"></script>
</body>

</html>
''')