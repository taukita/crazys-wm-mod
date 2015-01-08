/*
* Copyright 2009, 2010, The Pink Petal Development Team.
* The Pink Petal Devloment Team are defined as the game's coders
* who meet on http://pinkpetal.org     // old site: http://pinkpetal .co.cc
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "cJobManager.h"
#include "cBrothel.h"
#include "cCustomers.h"
#include "cRng.h"
#include "cInventory.h"
#include "sConfig.h"
#include "cRival.h"
#include <sstream>
#include "CLog.h"
#include "cTrainable.h"
#include "cTariff.h"
#include "cGold.h"
#include "cGangs.h"
#include "cMessageBox.h"

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;


// `J` Brothel Job - Brothel
bool cJobManager::WorkBrothelStripper(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	string message = ""; string warning = ""; string girlName = girl->m_Realname;
	if (Preprocessing(ACTION_WORKSTRIP, girl, brothel, Day0Night1, summary, message))
		return true;

	// put that shit away, you'll scare off the customers!
	g_Girls.UnequipCombat(girl);

	int roll = g_Dice % 100;
	int jobperformance = (g_Girls.GetStat(girl, STAT_CHARISMA) / 4 +
		g_Girls.GetStat(girl, STAT_BEAUTY) / 4 +
		g_Girls.GetSkill(girl, SKILL_PERFORMANCE) / 2 +
		g_Girls.GetSkill(girl, SKILL_STRIP));
	int lapdance = (g_Girls.GetStat(girl, STAT_INTELLIGENCE) / 2 +
		g_Girls.GetSkill(girl, SKILL_PERFORMANCE) / 2 +
		g_Girls.GetSkill(girl, SKILL_STRIP)) / 2;
	int mast = false, sex = false;
	int wages = 45, work = 0;

	message = girlName + " was stripping in the brothel.";
	if (g_Girls.GetStat(girl, STAT_BEAUTY) > 90)
	{
		message += " She is so hot, customers were waving money to attract her to dance for them.";
		wages += 20;
	}
	if (g_Girls.GetStat(girl, STAT_INTELLIGENCE) > 75)
	{
		message += " She was smart enough to boost her pay by playing two customers off against one another.";
		wages += 25;
	}
	message += "\nShe stripped for a customer.\n\n";

	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))   jobperformance += 15;
	if (g_Girls.HasTrait(girl, "Sexy Air"))		 jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Cool Person"))   jobperformance += 10; //people love to be around her
	if (g_Girls.HasTrait(girl, "Cute"))			 jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Charming"))		 jobperformance += 10; //people like charming people
	if (g_Girls.HasTrait(girl, "Great Figure"))	 jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Great Arse"))	 jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Quick Learner")) jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Psychic"))		 jobperformance += 10; //knows what people want
	if (g_Girls.HasTrait(girl, "Long Legs"))	 jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Exhibitionist")) jobperformance += 10; //SIN - likes showing off her body
	if (g_Girls.GetStat(girl, STAT_FAME) > 85)	 jobperformance += 10; //more people willing to see her


	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))		jobperformance -= 50; //needs others to do the job
	if (g_Girls.HasTrait(girl, "Clumsy"))			jobperformance -= 20; //spills food and breaks things often
	if (g_Girls.HasTrait(girl, "Aggressive"))		jobperformance -= 20; //gets mad easy and may attack people
	if (g_Girls.HasTrait(girl, "Nervous"))			jobperformance -= 30; //don't like to be around people
	if (g_Girls.HasTrait(girl, "Meek"))				jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Shy"))				jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Slow Learner"))		jobperformance -= 10;
	if (g_Girls.HasTrait(girl, "Horrific Scars"))	jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Small Scars"))		jobperformance -= 5;
	if (girl->is_pregnant())						jobperformance -= 10; //SIN - can't move so well

	//Adding cust here for use in scripts...
	sCustomer cust;
	GetMiscCustomer(brothel, cust);

	//A little more randomness
	if ((cust.m_IsWoman) && ((g_Girls.HasTrait(girl, "Lesbian") || g_Girls.GetSkill(girl, SKILL_LESBIAN) > 60)))
	{
		message += girlName + " was overjoyed to perform for a woman, and gave a much more sensual, personal performance.";
		jobperformance += 25;
	}
	if (g_Girls.GetStat(girl, STAT_TIREDNESS) > 70)
	{
		message += girlName + " was too exhausted to give her best tonight";
		if (jobperformance >= 120)
		{
			message += ", but she did a fairly good job of hiding her exhaustion.\n";
			jobperformance -= 10;
		}
		else
		{
			message += ". Unable to even mask her tiredness, she moved clumsily and openly yawned around customers.\n";
			jobperformance -= 30;
		}
	}

	if (jobperformance >= 245)
	{
		message += " She must be the perfect stripper - customers go on and on about her and always come to see her when she works.\n\n";
		wages += 155;
	}
	else if (jobperformance >= 185)
	{
		message += " She's unbelievable at this and is always getting praised by the customers for her work.\n\n";
		wages += 95;
	}
	else if (jobperformance >= 145)
	{
		message += " She's good at this job and gets praised by the customers often.\n\n";
		wages += 55;
	}
	else if (jobperformance >= 100)
	{
		message += " She made a few mistakes but overall she is okay at this.\n\n";
		wages += 15;
	}
	else if (jobperformance >= 70)
	{
		message += " She was nervous and made a few mistakes. She isn't that good at this.\n\n";
		wages -= 5;
	}
	else
	{
		message += " She was nervous and constantly making mistakes. She really isn't very good at this job.\n\n";
		wages -= 15;
	}


	// lap dance code.. just test stuff for now
	if (lapdance >= 90)
	{
		message += girlName + " doesn't have to try to sell private dances the patrons beg her to buy one off her.\n";
		if (roll < 5)
		{
			message += "She sold a champagne dance.";
			wages += 250;
		}
		if (roll < 20)
		{
			message += "She sold a shower dance.\n";
			wages += 125;
			if (g_Girls.GetStat(girl, STAT_LIBIDO) > 70)
			{
				message += "She was in the mood so she put on quite a show, taking herself to orgasm right in front of the customer.";
				g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -20);
				wages += 50;
				mast = true;
			}
		}
		if (roll < 40)
		{
			message += "She was able to sell a few VIP dances.\n";
			wages += 160;
			if (g_Dice.percent(20))
			{
				sex = true;
			}
		}
		if (roll < 60)
		{
			message += "She sold a VIP dance.\n";
			wages += 75;
			if (g_Dice.percent(15))
			{
				sex = true;
			}
		}
		else
		{
			message += "She sold several lap dances.";
			wages += 85;
		}
	}
	else if (lapdance >= 65)
	{
		message += girlName + "'s skill at selling private dances is impressive.\n";
		if (roll < 10)
		{
			message += "She convinced a patron to buy a shower dance.\n";
			wages += 75;
			if (g_Girls.GetStat(girl, STAT_LIBIDO) > 70)
			{
				message += "She was in the mood so she put on quite a show, taking herself to orgasm right in front of the customer.";
				g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -20);
				wages += 50;
				mast = true;
			}
		}
		if (roll < 40)
		{
			message += "Sold a VIP dance to a patron.\n";
			wages += 75;
			if (g_Dice.percent(20))
			{
				sex = true;
			}
		}
		else
		{
			message += "Sold a few lap dances.";
			wages += 65;
		}
	}
	else if (lapdance >= 40)
	{
		message += girlName + " tried to sell private dances and ";
		if (roll < 5)
		{
			message += "was able to sell a vip dance againts all odds.\n";
			wages += 75;
			if (g_Dice.percent(10))
			{
				sex = true;
			}
		}
		if (roll < 20)
		{
			message += "was able to sell a lap dance.";
			wages += 25;
		}
		else
		{
			message += "wasn't able to sell any.";
		}
	}
	else
	{
		message += girlName + "'s doesn't seem to understand the real money in stripping is selling private dances.\n";
	}


	//try and add randomness here
	if (g_Girls.GetStat(girl, STAT_BEAUTY) > 85 && g_Dice.percent(20))
	{ message += "Stunned by her beauty a customer left her a great tip.\n\n"; wages += 25; }

	if (g_Girls.HasTrait(girl, "Clumsy") && g_Dice.percent(5))
		{ message += " Her clumsy nature caused her to slide off the pole causing her to have to stop stripping for a few hours.\n"; wages -= 15; }

	if (g_Girls.HasTrait(girl, "Pessimist") && g_Dice.percent(5))
	{
		if (jobperformance < 125)
			{ message += " Her pessimistic mood depressed the customers making them tip less.\n"; wages -= 10; }
		else
			{ message += girlName + " was in a poor mood so the patrons gave her a bigger tip to try and cheer her up.\n"; wages += 10; }
	}

	if (g_Girls.HasTrait(girl, "Optimist") && g_Dice.percent(5))
	{
		if (jobperformance < 125)
			{ message += girlName + " was in a cheerful mood but the patrons thought she needed to work more on her stripping.\n"; wages -= 10; }
		else
			{ message += " Her optimistic mood made patrons cheer up increasing the amount they tip.\n"; wages += 10; }
	}

	if (g_Girls.HasTrait(girl, "Great Figure") && g_Dice.percent(20))
	{
		if (jobperformance < 125)
			{ message += girlName + " has a great figure so she draws a few extra patrons even if she needed to work more on her stripping.\n"; wages += 5; }
		else
			{ message += girlName + "'s great figure draws a large crowed to the stage and her skill at stripping makes them pay up to see the show up close.\n"; wages += 15; }
	}

	//if (g_Dice.percent(10))//ruffe event
	//{
	//	message += "A patron keep buying her drinks \n";
	//	if (g_Girls.GetStat(girl, SKILL_HERBALISM) > 35)
	//	{
	//		message += "but she noticed an extra taste that she knew was a drug to make her pass out. She reported him to secuirty and he was escorted out. Good news is she made a good amount of money off him before this.\n"; wages += 25;
	//	}
	//	else
	//	{
	//		//guy gets to have his way with her
	//	}
	//}

	if (girl->is_addict() && !sex && !mast && g_Dice.percent(60)) //not going to get money or drugs any other way
	{
		message += "\nNoticing her addiction, a customer offered her drugs for a blowjob. She accepted, taking him out of sight of security and sucking him off for no money.\n";
		warning = "Noticing her addiction, a customer offered her drugs for a blowjob. She accepted, taking him out of sight of security and sucking him off for no money.\n";
		if (g_Girls.HasTrait(girl, "Shroud Addict"))
		{
			g_Girls.AddInv(girl, g_InvManager.GetItem("Shroud Mushroom"));
		}
		if (g_Girls.HasTrait(girl, "Fairy Dust Addict"))
		{
			g_Girls.AddInv(girl, g_InvManager.GetItem("Fairy Dust"));
		}
		if (g_Girls.HasTrait(girl, "Viras Blood Addict"))
		{
			g_Girls.AddInv(girl, g_InvManager.GetItem("Vira Blood"));
		}
		girl->m_Events.AddMessage(warning, IMGTYPE_ORAL, EVENT_WARNING);
	}

	if (wages < 0)
		wages = 0;


	if (sex)
	{
		u_int n;
		message += "In one of the private shows, she ended up ";
		//sCustomer cust;  //SIN moved this to earlier.
		brothel->m_Happiness += 100;
		//GetMiscCustomer(brothel, cust);
		int imageType = IMGTYPE_SEX;
		if (cust.m_IsWoman)
		{
			n = SKILL_LESBIAN;
			message += "licking the customer's clit until she screamed out in pleasure, making her very happy.";
		}
		else
		{
			switch (g_Dice % 10)
			{
			case 0:        n = SKILL_ORALSEX;   message += "sucking the customer off";					break;
			case 1:        n = SKILL_TITTYSEX;  message += "using her tits to get the customer off";    break;
			case 2:        n = SKILL_HANDJOB;   message += "using her hand to get the customer off";    break;
			case 3:        n = SKILL_ANAL;      message += "letting the customer use her ass";			break;
			case 4:        n = SKILL_FOOTJOB;   message += "using her feet to get the customer off";    break;
			default:	   n = SKILL_NORMALSEX; message += "fucking the customer as well";				break;
			}
			message += ", making him very happy.\n";
		}
		/* */if (n == SKILL_LESBIAN)	imageType = IMGTYPE_LESBIAN;
		else if (n == SKILL_ORALSEX)	imageType = IMGTYPE_ORAL;
		else if (n == SKILL_TITTYSEX)	imageType = IMGTYPE_TITTY;
		else if (n == SKILL_HANDJOB)	imageType = IMGTYPE_HAND;
		else if (n == SKILL_FOOTJOB)	imageType = IMGTYPE_FOOT;
		else if (n == SKILL_ANAL)		imageType = IMGTYPE_ANAL;
		else if (n == SKILL_NORMALSEX)	imageType = IMGTYPE_SEX;
		g_Girls.UpdateSkill(girl, n, 2);
		g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -25);
		g_Girls.UpdateEnjoyment(girl, ACTION_SEX, +1, true);
		// work out the pay between the house and the girl
		wages += g_Girls.GetStat(girl, STAT_ASKPRICE);
		int roll_max = (g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CHARISMA));
		roll_max /= 4;
		wages += 50 + g_Dice%roll_max;
		girl->m_Pay = wages;
		girl->m_Events.AddMessage(message, imageType, Day0Night1);
	}
	else if (mast)
	{
		brothel->m_Happiness += (g_Dice % 70) + 60;
		// work out the pay between the house and the girl
		int roll_max = (g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CHARISMA));
		roll_max /= 4;
		wages += 50 + g_Dice%roll_max;
		girl->m_Pay = wages;
		girl->m_Events.AddMessage(message, IMGTYPE_MAST, Day0Night1);
	}
	else
	{
		brothel->m_Happiness += (g_Dice % 70) + 30;
		// work out the pay between the house and the girl
		int roll_max = (g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CHARISMA));
		roll_max /= 4;
		wages += 10 + g_Dice%roll_max;
		girl->m_Pay = wages;
		girl->m_Events.AddMessage(message, IMGTYPE_STRIP, Day0Night1);
	}


	//enjoyed the work or not
	if (roll <= 5)
	{ message += "\nSome of the patrons abused her during the shift."; work -= 1; }
	else if (roll <= 25)
	{ message += "\nShe had a pleasant time working."; work += 3; }
	else
	{ message += "\nOtherwise, the shift passed uneventfully."; work += 1; }

	g_Girls.UpdateEnjoyment(girl, ACTION_WORKSTRIP, work, true);


	// Improve stats
	int xp = 15, libido = 1, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_FAME, 1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_STRIP, g_Dice%skill + 2);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	//gained
	g_Girls.PossiblyGainNewTrait(girl, "Sexy Air", 80, ACTION_WORKSTRIP, girlName + " has been stripping and having to be sexy for so long she now reeks of sexyness.", Day0Night1 == SHIFT_NIGHT);

	//lose
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 30, ACTION_WORKSTRIP, girlName + " has had so many people see her naked she is no longer nervous about anything.", Day0Night1 == SHIFT_NIGHT);
	if (jobperformance > 150 && g_Girls.GetStat(girl, STAT_CONFIDENCE) > 65) { g_Girls.PossiblyLoseExistingTrait(girl, "Shy", 60, ACTION_WORKSTRIP, girlName + " has been stripping for so long now that her confidence is super high and she is no longer Shy.", Day0Night1 == SHIFT_NIGHT); }

	return false;
}