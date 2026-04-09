#include <stdio.h>
#include <string.h>
#include <time.h>

#include <ldpasswd/ldpasswd.h>

int main(void) {
    char test_pws[][64] = {
        "musketeer333marshmallow",
        "!blah#Thing^",
        "Password7JamesTh1ngFather",
        "PassWord123",
        "Unknownword",
        "jamesblahblahblah",
        "P@sSw0Rd",
        "passw0rdRed14&TeamSucks",
        "123PassWord123",
        "1Unknown(((wordBlah",
        "P@sSw0RdJ4mes",
        "Change.me123!",
        "starburst",
        "PleaseDont5489CH32",
        "Super123!?",
        "VikingWorksAgain!",
        "RedTeamIsUgly!!",
        "White#Comet192@Map28",
        "WhyAreWeStillHere2015?",
        "foobarbaz5",
        "weloveyouredteam",
        "RickerBalls2",
        "Idkwhatimdoinghelp1",
        "ThankYouWhiteTeam",
        "Apple432Bottom#",
        "Welcome1",
        "RedTeamSucks3",
        "LemonJumpSlide1#",
        "AngleBatman1!",
        "IHateRedTeam1234",
        "ILoveWhiteTeam1234",
        "ThisIsAStrongPassword1234!",
        "P@ssw0rdIsStrong1234!",
        "changeme",
        "sysadmin",
        "letredin",
        "southycoder",
        "MYz=XfwE!ejCw?{D",
        "12345678910",
        "12345678",
        "weloveccdclinux",
        "helloredteam",
        "rootIsVeryImportant",
        "somedifferentpassword123",
        "M0diF13d19^",
        "6wVQQezTk3T7iT9l8dOJ",
        "375iY5zHMH14vrlQyMwX",
        "MyNameIsMax",
        "ILoveRedTeam",
        "ILoveRedTeam123",
        "FUCKRedTeam",
        "PLEASEINEEDTHEROUTER",
        "SvyatoslavSucks",
        "RedTeamIsTheWorstTeamEver12345",
        "SUPERsecurePASSWORD",
    };

    // Important, If you are trying to test in scale, uncomment this line and then comment out the same line in perturb.c
    // because when using in an actual envirnment, we want to randomize everytime, but for brute force testing, we only
    // want to randomize once.
    // srand(time(NULL));

    double epsilon_values[] = {40, 35, 30, 25, 20, 17.5, 15, 12.5, 10, 7.5, 5, 2.5, 1};
    for (int j = 0; j < 13; j++) {
        printf("Epsilon: %f\n", epsilon_values[j]);
        int num_same = 0;
        for (int i = 0; i < 54; i++) { //54
            for (int k = 0; k < 1000; k++) {
                char temp[64];
                strncpy(temp, test_pws[i], 64);
                perturb_password(temp, epsilon_values[j]);
                if (strcmp(temp, test_pws[i]) == 0) {
                    num_same++;
                }   
            }
        }
        printf("Percent Changed: %f\n\n", 100.0 * (54000 - num_same) / 54000);
    }
}