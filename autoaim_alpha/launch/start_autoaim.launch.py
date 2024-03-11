from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess

package_name = 'autoaim_alpha'
def generate_launch_description():
    return LaunchDescription([
        Node(
            package=package_name,
            node_executable='node_webcam_mv',
            node_name='node_webcam_mv',
            output='log'
        ),
        Node(
            package=package_name,
            node_executable='node_com',
            node_name='node_com',
            output='log'
            
        ),
        Node(
            package=package_name,
            node_executable='node_detector',
            node_name='node_detector',
            output='log'
            
        ),
        Node(
            package=package_name,
            node_executable='node_observer',
            node_name='node_observer',
            output='log'
            
        ),
        Node(
            package=package_name,
            node_executable='node_decision_maker',
            node_name='node_decision_maker',
            output='log'
            
        )
    ])
