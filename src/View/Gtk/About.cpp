/*
 * Copyright (C) 2010-2011 Daniel Richter <danielrichter2007@web.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include "About.h"

void View_Gtk_About::signal_about_dlg_response(int response_id){
	if (Gtk::RESPONSE_CLOSE)
		this->hide();
}

View_Gtk_About::View_Gtk_About()
	: appName("Grub Customizer"), appVersion(GC_VERSION)
{
	this->set_name(appName);
	this->set_version(appVersion);
	this->authors.push_back("Daniel Richter https://launchpad.net/~danielrichter2007");
	this->set_authors(authors);

	this->set_icon_name("grub-customizer");
	this->set_logo_icon_name("grub-customizer");
	this->set_comments(gettext("Grub Customizer is a graphical interface to configure the grub2/burg settings"));

	this->artists.push_back("Zelozelos");
	this->set_artists(artists);

	this->set_translator_credits(
"Adam Czabara https://launchpad.net/~adam-czabara\n\
Adolfo Jayme Barrientos https://launchpad.net/~fitoschido\n\
Alexander Gorishnyak https://launchpad.net/~kefir500\n\
Aleksey Kabanov Aleksey Kabanov\n\
Alessio Manai https://launchpad.net/~alessio-s\n\
Alexey Ivanov https://launchpad.net/~alexey.ivanes\n\
Algimantas Margevičius https://launchpad.net/~gymka\n\
Andrey Zaytsev https://launchpad.net/~andzaytsev\n\
Antón Méixome https://launchpad.net/~meixome\n\
Asier Iturralde Sarasola https://launchpad.net/~asier-iturralde\n\
ASTUR2000 https://launchpad.net/~astur2000\n\
Aurélien RIVIERE https://launchpad.net/~aurelien-riv\n\
Badver https://launchpad.net/~badver\n\
Bernardo Miguel Savone https://launchpad.net/~bersil\n\
Bonsoir https://launchpad.net/~bonsoir\n\
buba https://launchpad.net/~wilq1989\n\
Careone https://launchpad.net/~zzbusagain\n\
DAG Software https://launchpad.net/~dagsoftware\n\
Daniel Richter https://launchpad.net/~danielrichter2007\n\
David Jones https://launchpad.net/~bikerboi87\n\
Dimitar Dimitrov https://launchpad.net/~dimitrov\n\
Draky https://launchpad.net/~draky\n\
Eduardo Alberto Calvo https://launchpad.net/~edu5800\n\
Emre AYTAÇ https://launchpad.net/~eaytac\n\
enrigp https://launchpad.net/~enrgarc\n\
Erkin Batu Altunbaş https://launchpad.net/~erkin\n\
Eugene Marshal https://launchpad.net/~lowrider\n\
Eugênio F https://launchpad.net/~eugf\n\
Fedik https://launchpad.net/~fedikw\n\
Flames_in_Paradise https://launchpad.net/~ellisistfroh\n\
François Laurent https://launchpad.net/~francois.laurent\n\
František Zatloukal https://launchpad.net/~zatloukal-frantisek\n\
freedomrun https://launchpad.net/~freedomrun\n\
GamePad64 https://launchpad.net/~gamepad64\n\
Gianfranco Frisani https://launchpad.net/~gfrisani\n\
Gianluca https://launchpad.net/~albatrosslive\n\
gilles chaon https://launchpad.net/~g-chaon\n\
gogo https://launchpad.net/~trebelnik-stefina\n\
GoJoMo https://launchpad.net/~tolbkni\n\
harvalikjan@gmail.com https://launchpad.net/~harvalikjan\n\
Hriostat https://launchpad.net/~hriostat\n\
HSFF https://launchpad.net/~hsff\n\
Huan Peng https://launchpad.net/~penghuanmail\n\
I. De Marchi https://launchpad.net/~tangram-peces\n\
ivarela https://launchpad.net/~ivarela\n\
Jan Bažant aka Brbla https://launchpad.net/~brbla\n\
Jan Dolejš https://launchpad.net/~jackdown3csr\n\
Jarosław Ogrodnik https://launchpad.net/~goz\n\
jmb_kz https://launchpad.net/~jmb-kz\n\
Jörg BUCHMANN https://launchpad.net/~jorg-buchmann\n\
Jorge Luis Granda https://launchpad.net/~costeelation\n\
José Humberto Melo https://launchpad.net/~josehumberto-melo\n\
Juhani Numminen https://launchpad.net/~jsonic\n\
Julien https://launchpad.net/~julienmbpe\n\
Koshepa Alexander https://launchpad.net/~masheka3\n\
Lê Trường An. https://launchpad.net/~truongan\n\
Loic Treyvaud https://launchpad.net/~loictreyvaud\n\
londumas https://launchpad.net/~helion331990\n\
Maks Lyashuk aka Ma)(imuM https://launchpad.net/~probel\n\
Mantas Kriaučiūnas https://launchpad.net/~mantas\n\
Manuel Xosé Lemos https://launchpad.net/~mxlemos\n\
mario catena https://launchpad.net/~mcblackmar02\n\
Max_ym https://launchpad.net/~kontact-cat\n\
Maxime Gentils https://launchpad.net/~maxime.gentils\n\
Michael Konrad https://launchpad.net/~nephelyn\n\
Michael Kotsarinis https://launchpad.net/~mk73628\n\
Michele Dimaggio https://launchpad.net/~mike238dimaggio\n\
Miguel Anxo Bouzada https://launchpad.net/~mbouzada\n\
Mohamed Mohsen https://launchpad.net/~linuxer9\n\
Mohamed SEDKI https://launchpad.net/~sedki-mohamed\n\
Monkey https://launchpad.net/~monkey-libre\n\
Mustafa Yılmaz https://launchpad.net/~apshalasha\n\
nafterburner https://launchpad.net/~nafterburner\n\
Nenad Čubić https://launchpad.net/~nenad-cubic\n\
Octaiver Matt https://launchpad.net/~octaivermatt\n\
Ondrej Mosnáček https://launchpad.net/~omosnacek\n\
Papp Bence https://launchpad.net/~sclegnrbs\n\
patel https://launchpad.net/~patel\n\
Paulomorales https://launchpad.net/~paulocmorales\n\
Péter Trombitás https://launchpad.net/~trombipeti\n\
Prescott_SK https://launchpad.net/~prescott66\n\
Przemysław Buczkowski https://launchpad.net/~przemub\n\
rob https://launchpad.net/~rvdb\n\
Rodion R. https://launchpad.net/~r0di0n\n\
Şâkir Aşçı https://launchpad.net/~sakirasci\n\
shinichi imataka https://launchpad.net/~imatakas1\n\
shishimaru https://launchpad.net/~salvi-uchiha\n\
Stanz https://launchpad.net/~stanz\n\
Stavros K. Filippidis https://launchpad.net/~greatst\n\
Svetoslav Stefanov https://launchpad.net/~svetlisashkov\n\
taijuin lee https://launchpad.net/~taijuin\n\
TheMengzor https://launchpad.net/~the-mengzor\n\
Thibaut Panis https://launchpad.net/~thibaut-panis\n\
Tielhalter https://launchpad.net/~tielhalter\n\
Timo Seppola https://launchpad.net/~timo-seppola\n\
Tobi https://launchpad.net/~m-a-x-2-0-0-4\n\
Tolbkni Kao https://launchpad.net/~tolbkni\n\
Tomas Hasko https://launchpad.net/~thasko\n\
Tomislav Krznar https://launchpad.net/~tomislav-krznar\n\
Tubuntu https://launchpad.net/~t-ubuntu\n\
tzem https://launchpad.net/~athmakrigiannis\n\
ub https://launchpad.net/~xenolith0bytetestcomgreen\n\
Ulisses de C. Soares https://launchpad.net/~ulisses-soares\n\
Vizion-HUN https://launchpad.net/~vizler-k\n\
Vyacheslav Sharmanov https://launchpad.net/~vsharmanov\n\
Wang Dianjin https://launchpad.net/~tuhaihe\n\
Yuriy Oleksiychuk https://launchpad.net/~yurolex\n\
Zbyněk Schwarz https://launchpad.net/~tsbook\n\
zeugma https://launchpad.net/~sunder67\n\
Юрій Олексійчук https://launchpad.net/~yurolex\
");
	this->signal_response().connect(sigc::mem_fun(this, &View_Gtk_About::signal_about_dlg_response));
}

void View_Gtk_About::show(){
	Gtk::AboutDialog::show();
}
