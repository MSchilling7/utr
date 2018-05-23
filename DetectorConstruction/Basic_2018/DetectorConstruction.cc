/*
utr - Geant4 simulation of the UTR at HIGS
Copyright (C) 2017 the developing team (see README.md)

This file is part of utr.

utr is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

utr is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with utr.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "DetectorConstruction.hh"

// Materials
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "Materials.hh"
Materials *Materials::instance = NULL;
Materials *materials = Materials::Instance();

// Geometry
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "globals.hh"

#include "Room.hh"
#include "Beampipe_Upstream.hh"
#include "Beampipe_Downstream.hh"
#include "First_UTR_Wall.hh"
#include "First_Setup.hh"
#include "G3_Wall.hh"
#include "Detectors_G3.hh"
#include "Wheel.hh"
#include "G3_Table.hh"
#include "Table2.hh"
#include "Detectors_2nd.hh"
#include "ZeroDegree_Setup.hh"

// Sensitive Detectors
//#include "EnergyDepositionSD.hh"
//#include "G4SDManager.hh"
//#include "ParticleSD.hh"
//#include "SecondarySD.hh"

DetectorConstruction::DetectorConstruction() {}

DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume *DetectorConstruction::Construct() {

	/*
	 * Fast-forward to specific parts of the geometry by searching for
	 * ROOM (UTR walls and floor)
	 * WORLD (world volume)
	 * BEAMPIPE_UPSTREAM 
	 * BEAMPIPE_DOWNSTREAM
	 * FIRST_UTR_WALL
	 * FIRST_SETUP (first setup upstream of g3)
	 * G3_WALL (wall immediately in front of g3)
	 * DETECTORS_G3 (detectors in g3)
	 * WHEEL (g3 wheel)
	 * G3_TABLE
	 * TABLE_2 (the table on which the second setup is mounted)
	 * DETECTORS_2ND (detectors in second setup)
	 * ZERODEGREE_SETUP (zero-degree detector)
	 */

	G4Colour white(1.0, 1.0, 1.0);
	G4Colour grey(0.5, 0.5, 0.5);
	G4Colour black(0.0, 0.0, 0.0);
	G4Colour red(1.0, 0.0, 0.0);
	G4Colour green(0.0, 1.0, 0.0);
	G4Colour blue(0.0, 0.0, 1.0);
	G4Colour cyan(0.0, 1.0, 1.0);
	G4Colour magenta(1.0, 0.0, 1.0);
	G4Colour yellow(1.0, 1.0, 0.0);
	G4Colour orange(1.0, 0.5, 0.0);
	G4Colour light_orange(1.0, 0.82, 0.36);

	G4NistManager *nist = G4NistManager::Instance();

	G4Material *air = nist->FindOrBuildMaterial("G4_AIR");

	/***************** WORLD *****************/

	G4double World_x = 3000. * mm;
	G4double World_y = 3150. * mm;
	G4double World_z = 6000. * mm;

	G4Box *World_dim =
	    new G4Box("World_Solid", World_x * 0.5, World_y * 0.5, World_z * 0.5);

	G4LogicalVolume *World_Logical =
	    new G4LogicalVolume(World_dim, air, "World_Logical", 0, 0, 0);

	//World_Logical->SetVisAttributes(G4VisAttributes::GetInvisible());
    G4VisAttributes* world_vis = new G4VisAttributes(true, red);
    world_vis->SetForceWireframe(true);

	World_Logical->SetVisAttributes(world_vis);

	G4VPhysicalVolume *World_Physical =
	    new G4PVPlacement(0, G4ThreeVector(), World_Logical, "World", 0, false, 0);

	/***************** GENERAL DIMENSIONS *****************/

	G4double Wheel_To_Target = 3.*inch;
	G4double First_Setup_To_Wheel = 34.*inch;
	G4double First_UTR_Wall_To_First_Setup = 4.2*inch;
	G4double First_Setup_To_G3_Wall = 3.5*inch;
	G4double G3_Target_To_2nd_Target = 62.*inch; // Estimated
	G4double ZeroDegree_To_2nd_Target = 980.*mm;

	/***************** INITIALIZATIONS *****************/

	Beampipe_Upstream beampipe_Upstream;
	Beampipe_Downstream beampipe_Downstream;
	First_UTR_Wall first_UTR_Wall;
	First_Setup first_Setup;
	G3_Wall g3_Wall;
	Detectors_G3 detectors_G3;
	Wheel wheel;
	G3_Table g3_Table;
	Table2 table2;
	Detectors_2nd detectors_2nd;	
	ZeroDegree_Setup zeroDegree_Setup;
	Room room(World_x, World_y, World_z,
            Wheel_To_Target - First_Setup_To_Wheel - first_Setup.Get_Length() - First_UTR_Wall_To_First_Setup - first_UTR_Wall.Get_Length(),
	g3_Wall.Get_Floor_Level());

	/***************** ROOM *****************/

	new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), room.Get_Logical(), "Room", World_Logical, false, 0, false);

	/***************** UPSTREAM_BEAMPIPE *****************/

	new G4PVPlacement(0, G4ThreeVector(0., 0., beampipe_Upstream.Get_Z_Axis_Offset_Z()), beampipe_Upstream.Get_Logical(), "Beampipe_Upstream", World_Logical, false, 0, false);

	/***************** DOWNSTREAM_BEAMPIPE *****************/

	new G4PVPlacement(0, G4ThreeVector(0., 0., G3_Target_To_2nd_Target + beampipe_Downstream.Get_Z_Axis_Offset_Z()), beampipe_Downstream.Get_Logical(), "Beampipe_Downstream", World_Logical, false, 0, false);
	
    /***************** FIRST_UTR_WALL *****************/

	new G4PVPlacement(0, G4ThreeVector(0., 0., Wheel_To_Target - First_Setup_To_Wheel - first_Setup.Get_Length() - First_UTR_Wall_To_First_Setup - first_UTR_Wall.Get_Length()*0.5), first_UTR_Wall.Get_Logical(), "First_UTR_Wall", World_Logical, false, 0, false);

	/***************** FIRST_SETUP *****************/

	new G4PVPlacement(0, G4ThreeVector(0., 0., Wheel_To_Target - First_Setup_To_Wheel - first_Setup.Get_Length()*0.5), first_Setup.Get_Logical(), "First_Setup", World_Logical, false, 0, false);

	/***************** G3_WALL *****************/

	new G4PVPlacement(0, G4ThreeVector(0., 0., Wheel_To_Target - First_Setup_To_Wheel + First_Setup_To_G3_Wall + g3_Wall.Get_Length()*0.5), g3_Wall.Get_Logical(), "G3_Wall", World_Logical, false, 0, false);

	/***************** DETECTORS_G3 *****************/

	new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), detectors_G3.Get_Logical(), "Detectors_G3", World_Logical, false, 0, false);

	/***************** WHEEL *****************/

	new G4PVPlacement(0, G4ThreeVector(0., 0., Wheel_To_Target + wheel.Get_Length()*0.5), wheel.Get_Logical(), "Wheel", World_Logical, false, 0, false);

	/***************** G3_TABLE *****************/

	new G4PVPlacement(0, G4ThreeVector(0., 0., Wheel_To_Target + wheel.Get_Length() + g3_Table.Get_Length()*0.5), g3_Table.Get_Logical(), "G3_Table", World_Logical, false, 0, false);

	/***************** TABLE_2 *****************/

	new G4PVPlacement(0, G4ThreeVector(0., 0.,  Wheel_To_Target + wheel.Get_Length() + g3_Table.Get_Length() + table2.Get_Length()*0.5 + table2.Get_Z_Axis_Offset_Z()), table2.Get_Logical(), "Table2", World_Logical, false, 0, false);

	/***************** DETECTORS_2ND *****************/

	new G4PVPlacement(0, G4ThreeVector(0., 0., G3_Target_To_2nd_Target), detectors_2nd.Get_Logical(), "Detectors_2nd", World_Logical, false, 0, false);

	/***************** ZERODEGREE_SETUP *****************/

	new G4PVPlacement(0, G4ThreeVector(0., 0., G3_Target_To_2nd_Target + ZeroDegree_To_2nd_Target), zeroDegree_Setup.Get_Logical(), "ZeroDegree_Setup", World_Logical, false, 0, false);
	

	return World_Physical;
}

void DetectorConstruction::ConstructSDandField() {

}