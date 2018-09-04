/* ----------------------------------------------------------------------
* Copyright (C) 2010-2013 ARM Limited. All rights reserved.
*
* $Date:        17. January 2013
* $Revision:  V1.4.1
*
* Project:      CMSIS DSP Library
* Title:  arm_sin_cos_f32.c
*
* Description: Sine and Cosine calculation for floating-point values.
*
* Target Processor: Cortex-M4/Cortex-M3/Cortex-M0
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*   - Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   - Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in
*     the documentation and/or other materials provided with the
*     distribution.
*   - Neither the name of ARM LIMITED nor the names of its contributors
*     may be used to endorse or promote products derived from this
*     software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
* -------------------------------------------------------------------- */

#include "arm_math.h"

/**
 * @ingroup groupController
 */

/**
 * @defgroup SinCos Sine Cosine
 *
 * Computes the trigonometric sine and cosine values using a combination of table lookup
 * and linear interpolation.
 * There are separate functions for Q31 and floating-point data types.
 * The input to the floating-point version is in degrees while the
 * fixed-point Q31 have a scaled input with the range
 * [-1 0.9999] mapping to [-180 179] degrees.
 *
 * The implementation is based on table lookup using 360 values together with linear interpolation.
 * The steps used are:
 *  -# Calculation of the nearest integer table index.
 *  -# Compute the fractional portion (fract) of the input.
 *  -# Fetch the value corresponding to \c index from sine table to \c y0 and also value from \c index+1 to \c y1.
 *  -# Sine value is computed as <code> *psinVal = y0 + (fract * (y1 - y0))</code>.
 *  -# Fetch the value corresponding to \c index from cosine table to \c y0 and also value from \c index+1 to \c y1.
 *  -# Cosine value is computed as <code> *pcosVal = y0 + (fract * (y1 - y0))</code>.
 */

/**
* @addtogroup SinCos
* @{
*/
/**
* \par
* Cosine Table is generated from following loop
* <pre>for(i = 0; i < 360; i++)
* {
*    cosTable[i]= cos((i-180) * PI/180.0);
* } </pre>
*/

static const float32_t cosTable[360] = {
    -0.999847695156391270f, -0.999390827019095760f, -0.998629534754573830f,
    -0.997564050259824200f, -0.996194698091745550f, -0.994521895368273290f,
    -0.992546151641321980f, -0.990268068741570250f,
    -0.987688340595137660f, -0.984807753012208020f, -0.981627183447663980f,
    -0.978147600733805690f, -0.974370064785235250f, -0.970295726275996470f,
    -0.965925826289068200f, -0.961261695938318670f,
    -0.956304755963035440f, -0.951056516295153530f, -0.945518575599316740f,
    -0.939692620785908320f, -0.933580426497201740f, -0.927183854566787310f,
    -0.920504853452440150f, -0.913545457642600760f,
    -0.906307787036649940f, -0.898794046299167040f, -0.891006524188367790f,
    -0.882947592858926770f, -0.874619707139395740f, -0.866025403784438710f,
    -0.857167300702112220f, -0.848048096156425960f,
    -0.838670567945424160f, -0.829037572555041620f, -0.819152044288991580f,
    -0.809016994374947340f, -0.798635510047292940f, -0.788010753606721900f,
    -0.777145961456970680f, -0.766044443118977900f,
    -0.754709580222772010f, -0.743144825477394130f, -0.731353701619170460f,
    -0.719339800338651300f, -0.707106781186547460f, -0.694658370458997030f,
    -0.681998360062498370f, -0.669130606358858240f,
    -0.656059028990507500f, -0.642787609686539360f, -0.629320391049837280f,
    -0.615661475325658290f, -0.601815023152048380f, -0.587785252292473030f,
    -0.573576436351045830f, -0.559192903470746680f,
    -0.544639035015027080f, -0.529919264233204790f, -0.515038074910054270f,
    -0.499999999999999780f, -0.484809620246337000f, -0.469471562785890530f,
    -0.453990499739546750f, -0.438371146789077510f,
    -0.422618261740699330f, -0.406736643075800100f, -0.390731128489273600f,
    -0.374606593415912070f, -0.358367949545300270f, -0.342020143325668710f,
    -0.325568154457156420f, -0.309016994374947340f,
    -0.292371704722736660f, -0.275637355816999050f, -0.258819045102520850f,
    -0.241921895599667790f, -0.224951054343864810f, -0.207911690817759120f,
    -0.190808995376544800f, -0.173648177666930300f,
    -0.156434465040231040f, -0.139173100960065350f, -0.121869343405147370f,
    -0.104528463267653330f, -0.087155742747658235f, -0.069756473744125330f,
    -0.052335956242943620f, -0.034899496702500733f,
    -0.017452406437283477f, 0.000000000000000061f, 0.017452406437283376f,
    0.034899496702501080f, 0.052335956242943966f, 0.069756473744125455f,
    0.087155742747658138f, 0.104528463267653460f,
    0.121869343405147490f, 0.139173100960065690f, 0.156434465040230920f,
    0.173648177666930410f, 0.190808995376544920f, 0.207911690817759450f,
    0.224951054343864920f, 0.241921895599667900f,
    0.258819045102520740f, 0.275637355816999160f, 0.292371704722736770f,
    0.309016994374947450f, 0.325568154457156760f, 0.342020143325668820f,
    0.358367949545300380f, 0.374606593415911960f,
    0.390731128489273940f, 0.406736643075800210f, 0.422618261740699440f,
    0.438371146789077460f, 0.453990499739546860f, 0.469471562785890860f,
    0.484809620246337110f, 0.500000000000000110f,
    0.515038074910054380f, 0.529919264233204900f, 0.544639035015027200f,
    0.559192903470746790f, 0.573576436351046050f, 0.587785252292473140f,
    0.601815023152048270f, 0.615661475325658290f,
    0.629320391049837500f, 0.642787609686539360f, 0.656059028990507280f,
    0.669130606358858240f, 0.681998360062498480f, 0.694658370458997370f,
    0.707106781186547570f, 0.719339800338651190f,
    0.731353701619170570f, 0.743144825477394240f, 0.754709580222772010f,
    0.766044443118978010f, 0.777145961456970900f, 0.788010753606722010f,
    0.798635510047292830f, 0.809016994374947450f,
    0.819152044288991800f, 0.829037572555041620f, 0.838670567945424050f,
    0.848048096156425960f, 0.857167300702112330f, 0.866025403784438710f,
    0.874619707139395740f, 0.882947592858926990f,
    0.891006524188367900f, 0.898794046299167040f, 0.906307787036649940f,
    0.913545457642600870f, 0.920504853452440370f, 0.927183854566787420f,
    0.933580426497201740f, 0.939692620785908430f,
    0.945518575599316850f, 0.951056516295153530f, 0.956304755963035440f,
    0.961261695938318890f, 0.965925826289068310f, 0.970295726275996470f,
    0.974370064785235250f, 0.978147600733805690f,
    0.981627183447663980f, 0.984807753012208020f, 0.987688340595137770f,
    0.990268068741570360f, 0.992546151641321980f, 0.994521895368273290f,
    0.996194698091745550f, 0.997564050259824200f,
    0.998629534754573830f, 0.999390827019095760f, 0.999847695156391270f,
    1.000000000000000000f, 0.999847695156391270f, 0.999390827019095760f,
    0.998629534754573830f, 0.997564050259824200f,
    0.996194698091745550f, 0.994521895368273290f, 0.992546151641321980f,
    0.990268068741570360f, 0.987688340595137770f, 0.984807753012208020f,
    0.981627183447663980f, 0.978147600733805690f,
    0.974370064785235250f, 0.970295726275996470f, 0.965925826289068310f,
    0.961261695938318890f, 0.956304755963035440f, 0.951056516295153530f,
    0.945518575599316850f, 0.939692620785908430f,
    0.933580426497201740f, 0.927183854566787420f, 0.920504853452440370f,
    0.913545457642600870f, 0.906307787036649940f, 0.898794046299167040f,
    0.891006524188367900f, 0.882947592858926990f,
    0.874619707139395740f, 0.866025403784438710f, 0.857167300702112330f,
    0.848048096156425960f, 0.838670567945424050f, 0.829037572555041620f,
    0.819152044288991800f, 0.809016994374947450f,
    0.798635510047292830f, 0.788010753606722010f, 0.777145961456970900f,
    0.766044443118978010f, 0.754709580222772010f, 0.743144825477394240f,
    0.731353701619170570f, 0.719339800338651190f,
    0.707106781186547570f, 0.694658370458997370f, 0.681998360062498480f,
    0.669130606358858240f, 0.656059028990507280f, 0.642787609686539360f,
    0.629320391049837500f, 0.615661475325658290f,
    0.601815023152048270f, 0.587785252292473140f, 0.573576436351046050f,
    0.559192903470746790f, 0.544639035015027200f, 0.529919264233204900f,
    0.515038074910054380f, 0.500000000000000110f,
    0.484809620246337110f, 0.469471562785890860f, 0.453990499739546860f,
    0.438371146789077460f, 0.422618261740699440f, 0.406736643075800210f,
    0.390731128489273940f, 0.374606593415911960f,
    0.358367949545300380f, 0.342020143325668820f, 0.325568154457156760f,
    0.309016994374947450f, 0.292371704722736770f, 0.275637355816999160f,
    0.258819045102520740f, 0.241921895599667900f,
    0.224951054343864920f, 0.207911690817759450f, 0.190808995376544920f,
    0.173648177666930410f, 0.156434465040230920f, 0.139173100960065690f,
    0.121869343405147490f, 0.104528463267653460f,
    0.087155742747658138f, 0.069756473744125455f, 0.052335956242943966f,
    0.034899496702501080f, 0.017452406437283376f, 0.000000000000000061f,
    -0.017452406437283477f, -0.034899496702500733f,
    -0.052335956242943620f, -0.069756473744125330f, -0.087155742747658235f,
    -0.104528463267653330f, -0.121869343405147370f, -0.139173100960065350f,
    -0.156434465040231040f, -0.173648177666930300f,
    -0.190808995376544800f, -0.207911690817759120f, -0.224951054343864810f,
    -0.241921895599667790f, -0.258819045102520850f, -0.275637355816999050f,
    -0.292371704722736660f, -0.309016994374947340f,
    -0.325568154457156420f, -0.342020143325668710f, -0.358367949545300270f,
    -0.374606593415912070f, -0.390731128489273600f, -0.406736643075800100f,
    -0.422618261740699330f, -0.438371146789077510f,
    -0.453990499739546750f, -0.469471562785890530f, -0.484809620246337000f,
    -0.499999999999999780f, -0.515038074910054270f, -0.529919264233204790f,
    -0.544639035015027080f, -0.559192903470746680f,
    -0.573576436351045830f, -0.587785252292473030f, -0.601815023152048380f,
    -0.615661475325658290f, -0.629320391049837280f, -0.642787609686539360f,
    -0.656059028990507500f, -0.669130606358858240f,
    -0.681998360062498370f, -0.694658370458997030f, -0.707106781186547460f,
    -0.719339800338651300f, -0.731353701619170460f, -0.743144825477394130f,
    -0.754709580222772010f, -0.766044443118977900f,
    -0.777145961456970680f, -0.788010753606721900f, -0.798635510047292940f,
    -0.809016994374947340f, -0.819152044288991580f, -0.829037572555041620f,
    -0.838670567945424160f, -0.848048096156425960f,
    -0.857167300702112220f, -0.866025403784438710f, -0.874619707139395740f,
    -0.882947592858926770f, -0.891006524188367790f, -0.898794046299167040f,
    -0.906307787036649940f, -0.913545457642600760f,
    -0.920504853452440150f, -0.927183854566787310f, -0.933580426497201740f,
    -0.939692620785908320f, -0.945518575599316740f, -0.951056516295153530f,
    -0.956304755963035440f, -0.961261695938318670f,
    -0.965925826289068200f, -0.970295726275996470f, -0.974370064785235250f,
    -0.978147600733805690f, -0.981627183447663980f, -0.984807753012208020f,
    -0.987688340595137660f, -0.990268068741570250f,
    -0.992546151641321980f, -0.994521895368273290f, -0.996194698091745550f,
    -0.997564050259824200f, -0.998629534754573830f, -0.999390827019095760f,
    -0.999847695156391270f, -1.000000000000000000f
};

/**
* \par
* Sine Table is generated from following loop
* <pre>for(i = 0; i < 360; i++)
* {
*    sinTable[i]= sin((i-180) * PI/180.0);
* } </pre>
*/
static const float32_t sinTable[360] = {
    -0.017452406437283439f, -0.034899496702500699f, -0.052335956242943807f,
    -0.069756473744125524f, -0.087155742747658638f, -0.104528463267653730f,
    -0.121869343405147550f, -0.139173100960065740f,
    -0.156434465040230980f, -0.173648177666930280f, -0.190808995376544970f,
    -0.207911690817759310f, -0.224951054343864780f, -0.241921895599667730f,
    -0.258819045102521020f, -0.275637355816999660f,
    -0.292371704722737050f, -0.309016994374947510f, -0.325568154457156980f,
    -0.342020143325668880f, -0.358367949545300210f, -0.374606593415912240f,
    -0.390731128489274160f, -0.406736643075800430f,
    -0.422618261740699500f, -0.438371146789077290f, -0.453990499739546860f,
    -0.469471562785891080f, -0.484809620246337170f, -0.499999999999999940f,
    -0.515038074910054380f, -0.529919264233204900f,
    -0.544639035015026860f, -0.559192903470746900f, -0.573576436351046380f,
    -0.587785252292473250f, -0.601815023152048160f, -0.615661475325658400f,
    -0.629320391049837720f, -0.642787609686539470f,
    -0.656059028990507280f, -0.669130606358858350f, -0.681998360062498590f,
    -0.694658370458997140f, -0.707106781186547570f, -0.719339800338651410f,
    -0.731353701619170570f, -0.743144825477394240f,
    -0.754709580222771790f, -0.766044443118978010f, -0.777145961456971010f,
    -0.788010753606722010f, -0.798635510047292720f, -0.809016994374947450f,
    -0.819152044288992020f, -0.829037572555041740f,
    -0.838670567945424050f, -0.848048096156426070f, -0.857167300702112330f,
    -0.866025403784438710f, -0.874619707139395850f, -0.882947592858927100f,
    -0.891006524188367900f, -0.898794046299166930f,
    -0.906307787036650050f, -0.913545457642600980f, -0.920504853452440370f,
    -0.927183854566787420f, -0.933580426497201740f, -0.939692620785908430f,
    -0.945518575599316850f, -0.951056516295153640f,
    -0.956304755963035550f, -0.961261695938318890f, -0.965925826289068310f,
    -0.970295726275996470f, -0.974370064785235250f, -0.978147600733805690f,
    -0.981627183447663980f, -0.984807753012208020f,
    -0.987688340595137660f, -0.990268068741570360f, -0.992546151641322090f,
    -0.994521895368273400f, -0.996194698091745550f, -0.997564050259824200f,
    -0.998629534754573830f, -0.999390827019095760f,
    -0.999847695156391270f, -1.000000000000000000f, -0.999847695156391270f,
    -0.999390827019095760f, -0.998629534754573830f, -0.997564050259824200f,
    -0.996194698091745550f, -0.994521895368273290f,
    -0.992546151641321980f, -0.990268068741570250f, -0.987688340595137770f,
    -0.984807753012208020f, -0.981627183447663980f, -0.978147600733805580f,
    -0.974370064785235250f, -0.970295726275996470f,
    -0.965925826289068310f, -0.961261695938318890f, -0.956304755963035440f,
    -0.951056516295153530f, -0.945518575599316740f, -0.939692620785908320f,
    -0.933580426497201740f, -0.927183854566787420f,
    -0.920504853452440260f, -0.913545457642600870f, -0.906307787036649940f,
    -0.898794046299167040f, -0.891006524188367790f, -0.882947592858926880f,
    -0.874619707139395740f, -0.866025403784438600f,
    -0.857167300702112220f, -0.848048096156426070f, -0.838670567945423940f,
    -0.829037572555041740f, -0.819152044288991800f, -0.809016994374947450f,
    -0.798635510047292830f, -0.788010753606722010f,
    -0.777145961456970790f, -0.766044443118978010f, -0.754709580222772010f,
    -0.743144825477394240f, -0.731353701619170460f, -0.719339800338651080f,
    -0.707106781186547460f, -0.694658370458997250f,
    -0.681998360062498480f, -0.669130606358858240f, -0.656059028990507160f,
    -0.642787609686539250f, -0.629320391049837390f, -0.615661475325658180f,
    -0.601815023152048270f, -0.587785252292473140f,
    -0.573576436351046050f, -0.559192903470746900f, -0.544639035015027080f,
    -0.529919264233204900f, -0.515038074910054160f, -0.499999999999999940f,
    -0.484809620246337060f, -0.469471562785890810f,
    -0.453990499739546750f, -0.438371146789077400f, -0.422618261740699440f,
    -0.406736643075800150f, -0.390731128489273720f, -0.374606593415912010f,
    -0.358367949545300270f, -0.342020143325668710f,
    -0.325568154457156640f, -0.309016994374947400f, -0.292371704722736770f,
    -0.275637355816999160f, -0.258819045102520740f, -0.241921895599667730f,
    -0.224951054343865000f, -0.207911690817759310f,
    -0.190808995376544800f, -0.173648177666930330f, -0.156434465040230870f,
    -0.139173100960065440f, -0.121869343405147480f, -0.104528463267653460f,
    -0.087155742747658166f, -0.069756473744125302f,
    -0.052335956242943828f, -0.034899496702500969f, -0.017452406437283512f,
    0.000000000000000000f, 0.017452406437283512f, 0.034899496702500969f,
    0.052335956242943828f, 0.069756473744125302f,
    0.087155742747658166f, 0.104528463267653460f, 0.121869343405147480f,
    0.139173100960065440f, 0.156434465040230870f, 0.173648177666930330f,
    0.190808995376544800f, 0.207911690817759310f,
    0.224951054343865000f, 0.241921895599667730f, 0.258819045102520740f,
    0.275637355816999160f, 0.292371704722736770f, 0.309016994374947400f,
    0.325568154457156640f, 0.342020143325668710f,
    0.358367949545300270f, 0.374606593415912010f, 0.390731128489273720f,
    0.406736643075800150f, 0.422618261740699440f, 0.438371146789077400f,
    0.453990499739546750f, 0.469471562785890810f,
    0.484809620246337060f, 0.499999999999999940f, 0.515038074910054160f,
    0.529919264233204900f, 0.544639035015027080f, 0.559192903470746900f,
    0.573576436351046050f, 0.587785252292473140f,
    0.601815023152048270f, 0.615661475325658180f, 0.629320391049837390f,
    0.642787609686539250f, 0.656059028990507160f, 0.669130606358858240f,
    0.681998360062498480f, 0.694658370458997250f,
    0.707106781186547460f, 0.719339800338651080f, 0.731353701619170460f,
    0.743144825477394240f, 0.754709580222772010f, 0.766044443118978010f,
    0.777145961456970790f, 0.788010753606722010f,
    0.798635510047292830f, 0.809016994374947450f, 0.819152044288991800f,
    0.829037572555041740f, 0.838670567945423940f, 0.848048096156426070f,
    0.857167300702112220f, 0.866025403784438600f,
    0.874619707139395740f, 0.882947592858926880f, 0.891006524188367790f,
    0.898794046299167040f, 0.906307787036649940f, 0.913545457642600870f,
    0.920504853452440260f, 0.927183854566787420f,
    0.933580426497201740f, 0.939692620785908320f, 0.945518575599316740f,
    0.951056516295153530f, 0.956304755963035440f, 0.961261695938318890f,
    0.965925826289068310f, 0.970295726275996470f,
    0.974370064785235250f, 0.978147600733805580f, 0.981627183447663980f,
    0.984807753012208020f, 0.987688340595137770f, 0.990268068741570250f,
    0.992546151641321980f, 0.994521895368273290f,
    0.996194698091745550f, 0.997564050259824200f, 0.998629534754573830f,
    0.999390827019095760f, 0.999847695156391270f, 1.000000000000000000f,
    0.999847695156391270f, 0.999390827019095760f,
    0.998629534754573830f, 0.997564050259824200f, 0.996194698091745550f,
    0.994521895368273400f, 0.992546151641322090f, 0.990268068741570360f,
    0.987688340595137660f, 0.984807753012208020f,
    0.981627183447663980f, 0.978147600733805690f, 0.974370064785235250f,
    0.970295726275996470f, 0.965925826289068310f, 0.961261695938318890f,
    0.956304755963035550f, 0.951056516295153640f,
    0.945518575599316850f, 0.939692620785908430f, 0.933580426497201740f,
    0.927183854566787420f, 0.920504853452440370f, 0.913545457642600980f,
    0.906307787036650050f, 0.898794046299166930f,
    0.891006524188367900f, 0.882947592858927100f, 0.874619707139395850f,
    0.866025403784438710f, 0.857167300702112330f, 0.848048096156426070f,
    0.838670567945424050f, 0.829037572555041740f,
    0.819152044288992020f, 0.809016994374947450f, 0.798635510047292720f,
    0.788010753606722010f, 0.777145961456971010f, 0.766044443118978010f,
    0.754709580222771790f, 0.743144825477394240f,
    0.731353701619170570f, 0.719339800338651410f, 0.707106781186547570f,
    0.694658370458997140f, 0.681998360062498590f, 0.669130606358858350f,
    0.656059028990507280f, 0.642787609686539470f,
    0.629320391049837720f, 0.615661475325658400f, 0.601815023152048160f,
    0.587785252292473250f, 0.573576436351046380f, 0.559192903470746900f,
    0.544639035015026860f, 0.529919264233204900f,
    0.515038074910054380f, 0.499999999999999940f, 0.484809620246337170f,
    0.469471562785891080f, 0.453990499739546860f, 0.438371146789077290f,
    0.422618261740699500f, 0.406736643075800430f,
    0.390731128489274160f, 0.374606593415912240f, 0.358367949545300210f,
    0.342020143325668880f, 0.325568154457156980f, 0.309016994374947510f,
    0.292371704722737050f, 0.275637355816999660f,
    0.258819045102521020f, 0.241921895599667730f, 0.224951054343864780f,
    0.207911690817759310f, 0.190808995376544970f, 0.173648177666930280f,
    0.156434465040230980f, 0.139173100960065740f,
    0.121869343405147550f, 0.104528463267653730f, 0.087155742747658638f,
    0.069756473744125524f, 0.052335956242943807f, 0.034899496702500699f,
    0.017452406437283439f, 0.000000000000000122f
};
/**
 * @brief  Floating-point sin_cos function.
 * @param[in]  theta    input value in degrees
 * @param[out] *pSinVal points to the processed sine output.
 * @param[out] *pCosVal points to the processed cos output.
 * @return none.
 */
void arm_sin_cos_f32(
    float32_t theta,
    float32_t * pSinVal,
    float32_t * pCosVal) {
    int32_t i;                                     /* Index for reading nearwst output values */
    float32_t x1 = -179.0f;                        /* Initial input value */
    float32_t y0, y1;                              /* nearest output values */
    float32_t y2, y3;
    float32_t fract;                               /* fractional part of input */

    /* Calculation of fractional part */
    if(theta > 0.0f) {
        fract = theta - (float32_t) ((int32_t) theta);
    } else {
        fract = (theta - (float32_t) ((int32_t) theta)) + 1.0f;
    }

    /* index calculation for reading nearest output values */
    i = (uint32_t) (theta - x1);

    /* Checking min and max index of table */
    if(i < 0) {
        i = 0;
    } else if(i >= 359) {
        i = 358;
    }

    /* reading nearest sine output values */
    y0 = sinTable[i];
    y1 = sinTable[i + 1u];

    /* reading nearest cosine output values */
    y2 = cosTable[i];
    y3 = cosTable[i + 1u];

    y1 = y1 - y0;
    y3 = y3 - y2;

    y1 = fract * y1;
    y3 = fract * y3;

    /* Calculation of sine value */
    *pSinVal = y0 + y1;

    /* Calculation of cosine value */
    *pCosVal = y2 + y3;

}

/**
 * @} end of SinCos group
 */
