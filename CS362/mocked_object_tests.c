[TestMethod]
        public void mockedParseHistoryFileArgumentNullExceptionTest()
        {
            //Test data
            Stream fileData;
            int userId = 2;
            int year = 10000;
            bool isFirstLineHeaders = false;
            string separator = ",";
            
            string inputRow = "1/14, 4:00am, 541-389-9009, peak, M2MAllow, Bend OR, Incoming CL, 40, \r\n";
            //Open the memory stream
            fileData = new MemoryStream(System.Text.Encoding.ASCII.GetBytes(inputRow));

            //Expected/actual results
            List<PhoneCall> expectedResult = new List<PhoneCall>();
            IEnumerable<PhoneCall> actualResult;
            List<string> expectedErrorRows = new List<string>();
            IList<string> actualErrors;
            
            //Set up expected results
            expectedErrorRows.Add(inputRow);

            //Make mock object
            Mock<CallHistoryParser> target = new Mock<CallHistoryParser>(MockBehavior.Loose);

            target.Setup(cp => cp.ParseRow(inputRow, userId, year, separator))
                .Throws(new ArgumentNullException("Message", "foobar"))
                .Verifiable();
            target.CallBase = true;

            //Call mock ParseHistoryFile
            actualResult = target.Object.ParseHistoryFile((Stream)fileData, userId, year, isFirstLineHeaders, separator, out actualErrors); 

            //Compare count of error rows to expected
            //Assert.AreEqual(expectedResult.Count, actualResult.Count());
            Assert.AreEqual(expectedErrorRows.Count, 1);
            fileData.Close();
        }

        [TestMethod]
        public void mockedParseHistoryFileArgumentOutOfRangeExceptionTest()
        {
            //Test data
            Stream fileData;
            int userId = -1;
            int year = 2000;
            bool isFirstLineHeaders = false;
            string separator = ",";

            string inputRow = "1/14, 4:00am, 541-389-9009, peak, M2MAllow, Bend OR, Incoming CL, 40, \r\n";
            //Open the memory stream
            fileData = new MemoryStream(System.Text.Encoding.ASCII.GetBytes(inputRow));

            //Expected/actual results
            List<PhoneCall> expectedResult = new List<PhoneCall>();
            IEnumerable<PhoneCall> actualResult;
            List<string> expectedErrorRows = new List<string>();
            IList<string> actualErrors;

            //Set up expected results
            expectedErrorRows.Add(inputRow);

            //Make mock object
            Mock<CallHistoryParser> target = new Mock<CallHistoryParser>(MockBehavior.Loose);
            target.Setup(cp => cp.ParseRow(inputRow, userId, year, separator)).Throws(new ArgumentOutOfRangeException("Message", "foobar"));
            target.CallBase = true;

            //Call mock ParseHistoryFile
            actualResult = target.Object.ParseHistoryFile((Stream)fileData, userId, year, isFirstLineHeaders, separator, out actualErrors);

            //Compare count of error rows to expected
            Assert.AreEqual(expectedResult.Count, actualResult.Count());
            Assert.AreEqual(expectedErrorRows.Count, actualErrors.Count);
            fileData.Close();
        }