# CPP Multi-BruteForce-Algorithm

&nbsp; Parallel Brute Force Algorithm made in cpp. Implemented as master-slave model, in which a client  aconnects and sends the required info
(IP , PORT, USERNAME, PASSWORDLIST). When master receives this values from the client ,  he forwards to all the slaves the values IP , PORT, USERNAME.
For each of the slaves, master makes a subset from the PASSWORDLIST and pass it to slave(let subset of PASSWORDLIST be PASSWORDLISTsubset). Each slave, open threads and sends a get request to IP PORT with the
username and a password from PASSWORDLISTsubset). Slave opens threads and tries passwords from the PASSWORDLISTsubset, until the PASSWORDLISTsubset have no more elements or the correct password is found.

## Getting Started[TODO]

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites[TODO]

What things you need to install the software and how to install them

```
Give examples
```

### Installing[TODO]

A step by step series of examples that tell you how to get a development env running

Say what the step will be

```
Give the example
```

And repeat

```
until finished
```

End with an example of getting some data out of the system or using it for a little demo

## Running the tests[TODO]

Explain how to run the automated tests for this system

### Break down into end to end tests[TODO]

Explain what these tests test and why

```
Give an example
```

### And coding style tests[TODO]

Explain what these tests test and why

```
Give an example
```

## Deployment[TODO]

Add additional notes about how to deploy this on a live system

## Built With[TODO]

* [Dropwizard](http://www.dropwizard.io/1.0.2/docs/) - The web framework used
* [Maven](https://maven.apache.org/) - Dependency Management
* [ROME](https://rometools.github.io/rome/) - Used to generate RSS Feeds

## Contributing[TODO]

Please read [CONTRIBUTING.md](https://gist.github.com/PurpleBooth/b24679402957c63ec426) for details on our code of conduct, and the process for submitting pull requests to us.

## Versioning[TODO]

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/your/project/tags). 

## Authors[TODO]

* **Billie Thompson** - *Initial work* - [PurpleBooth](https://github.com/PurpleBooth)

See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.

## License[TODO]

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments[TODO]

* Hat tip to anyone whose code was used
* Inspiration
* etc

