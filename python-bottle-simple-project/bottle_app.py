
#####################################################################
### Assignment skeleton
### You can alter the below code to make your own dynamic website.
### The landing page for assignment 3 should be at /
#####################################################################

from bottle import route, run, default_app, debug
import csv
from bottle import get, post, request




def htmlify(title,text):
    page = """
        <!doctype html>
        <html lang="en">
            <head>
                <meta charset="utf-8" />
                <title>%s</title>
				<style>
					ul {
						list-style-type: none;
						margin: 0;
						padding: 0;
						overflow: hidden;
						background-color: #333;
					}

					li {
						float: left;
						text-align: center;
					}

					li a {
						display: block;
						color: white;
						text-align: center;
						padding: 14px 16px;
						text-decoration: none;
					}

					li a:hover {
						background-color: #111;
					}
					table, td, th {    
						border: 1px solid #ddd;
						text-align: left;
					}

					table {
						border-collapse: collapse;
						width: 100%%;
					}

					th, td {
						padding: 15px;
					}
					tr:nth-child(even){background-color: #f2f2f2}
				</style>
            </head>
            <body>
				<div class="menu">
					<ul>
						<li><a href="/">Home</a></li>      
						<li><a href="/table">Table</a></li>
						<li><a href="/search">Searching Table</a></li>
						<li><a href="/filter">Filter Table</a></li>
						<li><a href="/register">Register</a></li>
					</ul>
                </div>
				%s
            </body>
        </html>

    """ % (title,text)
    return page

def index():
	text =	"""
			<h1> Home </h1> 
			<h2> Welcome to home page of Interpol Most Wanted List </h2>
			<h3> Writer : Cemal Turkoglu </h3>
			"""
	return htmlify("Interpol Most Wanted List",text)
	
def createTable():
	text = '<table>'
	with open('Countries.csv') as csvfile:
		readCSV = csv.reader(csvfile, delimiter=',')
		counter=0
		for row in readCSV:
			
			text += "<tr>"	
			
			for col in row:
				if counter==0:
					text += "<th>"
				else:
					text += "<td>"
				text += col
				if counter==0:
					text += "</th>"
				else:
					text += "</td>"
			
			text += "</tr>"	
			counter +=1
		text += "</table>"
	return text
	
def table():
	return htmlify("Interpol Most Wanted List",createTable())

def search():
	text = " Search content in the table "
	text = """ <form action="/search" method="post"> """
	text += """<table>
					<tr>
						<th>Search By Country</th>
						<th>Search By Region</th>
					</tr>
					<tr>
						<td> <input name="country" type="text" /> </td>
						<td> <input name="region" type="text" /> </td>
					</tr>
					<tr>
						<td> <input value="search" type="submit" /> </td>
						<td> <input value="search" type="submit" /> </td>
					</tr>
				</table>
			"""
	text += "</form> <br><br> <h3> Table: </h3>"
	text += createTable()
	return htmlify("Interpol Most Wanted List",text)
	
def doSearch():
	text = " Search content in the table "
	text += """ <form action="/search" method="post"> """
	text += """<table>
					<tr>
						<th>Search By Country</th>
						<th>Search By Region</th>
					</tr>
					<tr>
						<td> <input name="country" type="text" /> </td>
						<td> <input name="region" type="text" /> </td>
					</tr>
					<tr>
						<td> <input value="search" type="submit" /> </td>
						<td> <input value="search" type="submit" /> </td>
					</tr>
				</table>
			"""
	text += "</form> <br><br> <h3> Table: </h3>"
	if request.forms.get('country'):
		country = request.forms.get('country')
		text = '<table>'
		with open('Countries.csv') as csvfile:
			readCSV = csv.reader(csvfile, delimiter=',')
			for row in readCSV:
				if row[0].lower() == country.lower():
					text += "<tr>"	
					
					for col in row:
						
						text += "<td>"
						text += col
						text += "</td>"
					
					text += "</tr>"
			text += "</table>"
		return htmlify("Interpol Most Wanted List",text)
		
	elif request.forms.get('region'):
		region = request.forms.get('region')
		text = '<table>'
		with open('Countries.csv') as csvfile:
			readCSV = csv.reader(csvfile, delimiter=',')
			for row in readCSV:
				if row[4].lower() == region.lower():
					text += "<tr>"	
					
					for col in row:
						
						text += "<td>"
						text += col
						text += "</td>"
					
					text += "</tr>"
			text += "</table>"
		return htmlify("Interpol Most Wanted List",text)
	text += createTable()
	return htmlify("Interpol Most Wanted List",text)
	
def register():
	if request.GET.get('name', '').strip():
		text = "<h1>Welcome"+name+"</h1>"
		return htmlify("Interpol Most Wanted List",text)
	else:
		text = ' <form action="/register" method="post"> '
		text += """<table>
						<tr>
							<th>Register</th>
							<th></th>
						</tr>
						<tr>
							<td> Name </td>
							<td> <input name="name" type="text" /> </td>
						</tr>
						<tr>
							<td> Surname </td>
							<td> <input name="surname" type="text" /> </td>
						</tr>
						<tr>
							<td> Email </td>
							<td> <input name="surname" type="text" /> </td>
						</tr>
						<tr>
							<td> Gender </td>
							<td> Male &nbsp;&nbsp; <input name="male" type="radio" /><br>
								Female <input name="female" type="radio" /></td>
							
						</tr>
						<tr>
							<td> Remarks </td>
							<td>  <textarea name="msg" rows="4" cols="50"> </textarea></td>
							
						</tr>
						<tr>
							<td> <input value="Submit" type="submit" /> </td>
						</tr>
					</table>
				"""
		return htmlify("Interpol Most Wanted List",text)
		
def doRegister():
	if request.forms.get('name'):
		text = "<h1>Welcome "+request.forms.get('name')+" </h1>"
		text += ".your data has been saved and you are subscribed to our website."
		return htmlify("Interpol Most Wanted List",text)

def filter():
	text = """<form action="/filter" method="get">
					Filter by:<br>
					<select name="filterMethod">
						<option value="country">Country</option>
						<option value="region">Region</option>
					</select><br>
					<input type="submit" value="Apply" /> 
				  </form> <br>
				"""
	if 'filterMethod' in request.GET:
		text += '<table>'
		with open('Countries.csv') as csvfile:
			readCSV = csv.reader(csvfile, delimiter=',')
			
			for row in readCSV:
				
				text += "<tr>"	
				if request.GET.get('filterMethod') == "country":
					text += "<td>" + row[0] + "</td>" + "<td>" + row[1] + "</td>" + "<td>" + row[2] + "</td>" + "<td>" + row[3] + "</td>"
				elif request.GET.get('filterMethod') == "region":
					text += "<td>" + row[4] + "</td>" + "<td>" + row[1] + "</td>" + "<td>" + row[2] + "</td>" + "<td>" + row[3] + "</td>"
				text += "</tr>"	
				
			text += "</table>"
		return htmlify("Interpol Most Wanted List",text)
	else:
		
		
		text += createTable()
		return htmlify("Interpol Most Wanted List",text)
	
	
route('/', 'GET', index)
route('/table', 'GET', table)
route('/search', 'GET', search)
route('/search', 'POST', doSearch)
route('/register', 'GET', register)
route('/register', 'POST', doRegister)
route('/filter','GET',filter)
#####################################################################
### Don't alter the below code.
### It allows this website to be hosted on Heroku
### OR run on your computer.
#####################################################################

# This line makes bottle give nicer error messages
debug(True)
# This line is necessary for running on Heroku
app = default_app()
# The below code is necessary for running this bottle app standalone on your computer.
if __name__ == "__main__":
  run()

